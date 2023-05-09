﻿// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Server.h"
#include "ServerSocketHandler.h"
#include "Protocol/Messages/ClientAuthorizeMessage.h"
#include "Protocol/Messages/InvalidMessage.h"
#include "Protocol/Messages/ClientTextMessage.h"
#include "ServerUser.h"


std::vector<ServerUser*> Server::GetAllAuthorizedUsers()
{
    std::vector<ServerUser*> result;

    copy_if(users.begin(), users.end(), back_inserter(result), [](const ServerUser* user)
    {
        return user->IsAuthorized();
    });

    return result;
}


void Server::ReceiveMessage(Message* message, ServerSocketHandler* socketHandler)
{
    if (auto authMess = dynamic_cast<ClientAuthorizeMessage*>(message))
    {
        AuthorizeUser(*authMess, socketHandler);
    }
    else if (auto textMess = dynamic_cast<ClientTextMessage*>(message))
    {
        ReceiveText(*textMess, socketHandler);
    }
    else if (auto invMess = dynamic_cast<InvalidMessage*>(message))
    {
        std::cout << invMess->to_str() << std::endl;
    }
    else
    {
        std::cout << "Received unknown message" << std::endl;
    }
}


void Server::AuthorizeUser(ClientAuthorizeMessage& message, ServerSocketHandler* socketHandler)
{
    ServerUser* user = new ServerUser();
    user->id = ++last_user_id;
    user->nickname = message.user_name;
    user->socket_handlers.push_back(socketHandler);

    users.push_back(user);

    std::cout << "User authorized. Id: [" << user->id << "], name: [" << user->nickname << "]" << std::endl;

    socketHandler->SetUser(user);
    socketHandler->Send("A" + std::to_string(user->id) + "|" + user->nickname + ";\r\n");
}


void Server::ReceiveText(ClientTextMessage& message, ServerSocketHandler* socketHandler)
{
    const ServerUser* user = socketHandler->GetUser();
    if (!user || !user->IsAuthorized())
    {
        std::cout << "ERROR: Received text message from unauthorized user." << std::endl;
        socketHandler->Send("E|NOT_AUTHORIZED;\r\n");
        return;
    }

    //send message back to user
    std::cout << "TEXT from [" << user->nickname << "] : " << message.text << std::endl;
    // socketHandler->Send("R|" + std::to_string(message.text.size()) + ";\r\n");

    //broadcast message to all users
    const std::vector<ServerUser*> auth_users = GetAllAuthorizedUsers();
    for (const auto* auth_user : auth_users)
    {
        if (!auth_user) continue;

        for (auto* auth_user_socket : auth_user->socket_handlers)
        {
            if (!auth_user_socket) continue;

            auth_user_socket->Send(
                "T"
                + std::to_string(user->id) + "|"
                + user->nickname + "|"
                + message.text + ";\r\n");
        }
    }
}
