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
    authorized_users.erase(std::remove(authorized_users.begin(), authorized_users.end(), nullptr), authorized_users.end());

    return authorized_users;
}


void Server::ReceiveMessage(Message* message, ServerSocketHandler* socketHandler)
{
    if (const auto auth_mess = dynamic_cast<ClientAuthorizeMessage*>(message))
    {
        AuthorizeUser(*auth_mess, socketHandler);
    }
    else if (const auto text_mess = dynamic_cast<ClientTextMessage*>(message))
    {
        ReceiveText(*text_mess, socketHandler);
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


void Server::AuthorizeUser(ClientAuthorizeMessage& message, ServerSocketHandler* socketHandler)
{
    ServerUser* user = nullptr;

    auto it = users.find(message.user_name);
    if (it != users.end()) // если элемент найден
    {
        user = it->second; // извлекаем элемент
    }
    else
    {
        user = new ServerUser();
        user->id = ++last_user_id;
        user->user_name = message.user_name;
        users[message.user_name] = user;
    }
    user->socket_handlers.insert(socketHandler);


    std::cout
        << "User authorized. "
        << "Id: [" << user->id << "], "
        << "name: [" << user->user_name << "] "
        << "sockets: " << user->socket_handlers.size()
        << std::endl;

    socketHandler->SetUser(user);
    socketHandler->Send("A|" + std::to_string(user->id) + "|" + user->user_name + ";\r\n");
}


void Server::ReceiveText(ClientTextMessage& message, ServerSocketHandler* socketHandler)
{
    const ServerUser* user = socketHandler->GetUser();
    if (!user || !user->IsAuthorized())
    {
        std::cout << "ERROR: Received text message from unauthorized user." << std::endl;
        socketHandler->Send(ServerErrorMessage::NOT_AUTHORIZED);
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

        for (auto* auth_user_socket : auth_user->socket_handlers)
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
