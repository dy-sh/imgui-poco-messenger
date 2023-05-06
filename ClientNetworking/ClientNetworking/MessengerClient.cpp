// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessengerClient.h"
#include "ClientSocketHandler.h"
#include "Protocol/Messages/AuthorizeMessage.h"
#include "Protocol/Messages/InvalidMessage.h"
#include "Protocol/Messages/TextMessage.h"
#include "ClientUser.h"


std::vector<ClientUser*> MessengerClient::getAllAuthorizedUsers()
{
    std::vector<ClientUser*> result;

    copy_if(users.begin(), users.end(), back_inserter(result), [](const ClientUser* user)
    {
        return user->IsAuthorized();
    });

    return result;
}


void MessengerClient::receiveMessage(Message* message, ClientSocketHandler* socketHandler)
{
    if (auto authMess = dynamic_cast<AuthorizeMessage*>(message))
    {
        authorizeUser(*authMess, socketHandler);
    }
    else if (auto textMess = dynamic_cast<TextMessage*>(message))
    {
        receiveText(*textMess, socketHandler);
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


void MessengerClient::authorizeUser(AuthorizeMessage& message, ClientSocketHandler* socketHandler)
{
    ClientUser* user = new ClientUser();
    user->id = ++last_user_id;
    user->nickname = message.userName;
    user->socketHandlers.push_back(socketHandler);

    users.push_back(user);

    std::cout << "User authorized. Id: [" << user->id << "], name: [" << user->nickname << "]" << std::endl;

    socketHandler->SetUser(user);
    socketHandler->Send("AUTHORIZED|" + std::to_string(user->id) + "|" + user->nickname + ";\r\n");
}


void MessengerClient::receiveText(TextMessage& message, ClientSocketHandler* socketHandler)
{
    const ClientUser* user = socketHandler->GetUser();
    if (!user || !user->IsAuthorized())
    {
        std::cout << "ERROR: Received text message from unauthorized user." << std::endl;
        socketHandler->Send("ERROR|NOT_AUTHORIZED;\r\n");
        return;
    }

    //send message back to user
    std::cout << "TEXT from [" << user->nickname << "] : " << message.text << std::endl;
    socketHandler->Send("TEXT_RECEIVED|" + std::to_string(message.text.size()) + ";\r\n");

    //broadcast message to all users
    const std::vector<ClientUser*> auth_users = getAllAuthorizedUsers();
    for (const auto* auth_user : auth_users)
    {
        if (!auth_user) continue;

        for (auto* auth_user_socket : auth_user->socketHandlers)
        {
            if (!auth_user_socket) continue;

            auth_user_socket->Send(
                "TEXT|"
                + std::to_string(user->id) + "|"
                + user->nickname + "|"
                + message.text + ";\r\n");
        }
    }
}
