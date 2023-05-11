// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Server.h"

#include <Poco/Util/Application.h>

#include "ServerSocketHandler.h"
#include "Protocol/Messages/ClientAuthorizeMessage.h"
#include "Protocol/InvalidMessage.h"
#include "Protocol/Messages/ClientTextMessage.h"
#include "ServerUser.h"
#include "Protocol/Messages/ServerAuthorizeMessage.h"
#include "Protocol/Messages/ServerErrorMessage.h"
#include "Protocol/Messages/ServerJoinMessage.h"
#include "Protocol/Messages/ServerLeaveMessage.h"
#include "Protocol/Messages/ServerTextMessage.h"

using Poco::Util::Application;
using std::string;
using std::to_string;


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
        Application::instance().logger().error(inv_mess->to_str());
    }
    else
    {
        Application::instance().logger().error("Received unknown message");
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

        Application::instance().logger().information(
            "User disconnected. "
            "Id: [" + to_string(user->id) +
            " name: [" + user->user_name + "] "
            "sockets remain: " + to_string(user->sockets.size()));


        Application::instance().logger().information("Users on server: " + to_string(users.size()));

        string mess = ServerLeaveMessage::Serialize(user->id, user->user_name);
        Broadcast(mess);
    }
    else
    {
        Application::instance().logger().information("Unauthorized user disconnected");
    }
}


void Server::Broadcast(string message)
{
    const std::vector<ServerUser*> auth_users = GetAllAuthorizedUsers();
    for (const auto* auth_user : auth_users)
    {
        if (!auth_user) continue;

        for (auto* auth_user_socket : auth_user->sockets)
        {
            if (!auth_user_socket) continue;

            auth_user_socket->Send(message);
        }
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


    Application::instance().logger().information(
        "User authorized. "
        "Id: [" + to_string(user->id) + "], "
        "name: [" + user->user_name + "] "
        "sockets: " + to_string(user->sockets.size()));

    Application::instance().logger().information("Users on server: " + to_string(users.size()));

    socket_handler->SetUser(user);

    string mess = ServerAuthorizeMessage::Serialize(user->id, user->user_name);
    socket_handler->Send(mess);

    string mess2 = ServerJoinMessage::Serialize(user->id, user->user_name);
    Broadcast(mess2);
}


void Server::ReceiveText(ClientTextMessage& message, ServerSocketHandler* socket_handler)
{
    const ServerUser* user = socket_handler->GetUser();
    if (!user || !user->IsAuthorized())
    {
        Application::instance().logger().error("ERROR: Received text message from unauthorized user.");
        socket_handler->Send(ServerErrorMessage::NOT_AUTHORIZED);
        return;
    }

    Application::instance().logger().information("Received text message from [" + user->user_name + "]");

    //broadcast message to all users
    string mess = ServerTextMessage::Serialize(user->id, message.room_id, user->user_name, message.text);
    Broadcast(mess);
}
