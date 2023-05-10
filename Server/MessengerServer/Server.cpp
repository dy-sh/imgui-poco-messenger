// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Server.h"
#include "ServerSocketHandler.h"
#include "Protocol/Messages/ClientAuthorizeMessage.h"
#include "Protocol/Messages/InvalidMessage.h"
#include "Protocol/Messages/ClientTextMessage.h"
#include "ServerUser.h"
#include "Protocol/Messages/ServerErrorMessage.h"


std::vector<ServerUser*> Server::GetAllAuthorizedUsers()
{
    std::vector<ServerUser*> authorized_users;
    authorized_users.reserve(users.size());

    std::transform(users.begin(), users.end(), std::back_inserter(authorized_users), [](const auto& userPair)
    {
        if (userPair.second->IsAuthorized())
        {
            return userPair.second;
        }
        else
        {
            return static_cast<ServerUser*>(nullptr);
        }
    });

    //remove nullptrs
    authorized_users.erase(std::remove(authorized_users.begin(), authorized_users.end(), nullptr),
                           authorized_users.end());

    return authorized_users;
}


void Server::ReceiveMessage(Message* message, ServerSocketHandler* socket_handler)
{
    if (const auto auth_mess = dynamic_cast<ClientAuthorizeMessage*>(message))
    {
        AuthorizeUser(*auth_mess, socket_handler);
    }
    else if (const auto text_mess = dynamic_cast<ClientTextMessage*>(message))
    {
        ReceiveText(*text_mess, socket_handler);
    }
    else if (const auto inv_mess = dynamic_cast<InvalidMessage*>(message))
    {
        std::cout << inv_mess->to_str() << std::endl;
    }
    else
    {
        std::cout << "Received unknown message" << std::endl;
    }
}


void Server::OnSocketShutdown(ServerSocketHandler* socket_handler)
{
    auto it_user = users.find(socket_handler->GetUser()->user_name);
    if (it_user != users.end())
    {
        ServerUser* user = it_user->second;
        auto it = std::find(user->sockets.begin(), user->sockets.end(), socket_handler);
        if (it != user->sockets.end())
        {
            user->sockets.erase(it);
        }

        if (user->sockets.empty())
        {
            Poco::FastMutex::ScopedLock lock(users_mutex);
            users.erase(it_user);
        }

        std::cout
            << "User disconnected. "
            << "Id: [" << user->id << "], "
            << "name: [" << user->user_name << "] "
            << "sockets remain: " << user->sockets.size()
            << std::endl;

        std::cout << "Users on server: " << users.size() << std::endl;
    }
}


void Server::AuthorizeUser(ClientAuthorizeMessage& message, ServerSocketHandler* socket_handler)
{
    ServerUser* user = nullptr;

    auto it = users.find(message.user_name);
    if (it != users.end())
    {
        user = it->second;
    }
    else
    {
        user = new ServerUser();
        user->id = ++last_user_id;
        user->user_name = message.user_name;
        users[message.user_name] = user;
    }

    {
        Poco::FastMutex::ScopedLock lock(users_mutex);
        user->sockets.insert(socket_handler);
    }


    std::cout
        << "User authorized. "
        << "Id: [" << user->id << "], "
        << "name: [" << user->user_name << "] "
        << "sockets: " << user->sockets.size()
        << std::endl;

    std::cout << "Users on server: " << users.size() << std::endl;

    socket_handler->SetUser(user);
    socket_handler->Send("A|" + std::to_string(user->id) + "|" + user->user_name + ";\r\n");
}


void Server::ReceiveText(ClientTextMessage& message, ServerSocketHandler* socket_handler)
{
    const ServerUser* user = socket_handler->GetUser();
    if (!user || !user->IsAuthorized())
    {
        std::cout << "ERROR: Received text message from unauthorized user." << std::endl;
        socket_handler->Send(ServerErrorMessage::NOT_AUTHORIZED);
        return;
    }

    //send message back to user
    std::cout << "TEXT from [" << user->user_name << "] : " << message.text << std::endl;
    // socketHandler->Send("R|" + std::to_string(message.text.size()) + ";\r\n");


    //broadcast message to all users
    const std::vector<ServerUser*> auth_users = GetAllAuthorizedUsers();
    for (const auto* auth_user : auth_users)
    {
        if (!auth_user) continue;

        for (auto* auth_user_socket : auth_user->sockets)
        {
            if (!auth_user_socket) continue;

            std::cout << "!!!!  socket:" << &auth_user_socket << std::endl;
            auth_user_socket->Send(
                "T|"
                + std::to_string(user->id) + "|"
                + user->user_name + "|"
                + message.text + ";\r\n");
        }
    }
}
