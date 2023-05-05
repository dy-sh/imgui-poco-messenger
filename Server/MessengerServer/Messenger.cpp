// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Messenger.h"
#include "ServerSocketHandler.h"
#include "Protocol/Messages/AuthorizeMessage.h"
#include "Protocol/Messages/InvalidMessage.h"
#include "Protocol/Messages/TextMessage.h"
#include "User/User.h"


void Messenger::receiveMessage(Message* message, ServerSocketHandler* socketHandler)
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


void Messenger::authorizeUser(AuthorizeMessage& message, ServerSocketHandler* socketHandler)
{
    User* user = new User();
    user->id = ++last_user_id;
    user->nickname = message.userName;
    user->socketHandlers.push_back(socketHandler);

    users.push_back(user);

    std::cout << "User authorized. Id: [" << user->id << "], name: [" << user->nickname << "]" << std::endl;

    socketHandler->SetUser(user);
    socketHandler->Send("AUTHORIZED|" + std::to_string(user->id) + "|" + user->nickname + ";\r\n");
}


void Messenger::receiveText(TextMessage& message, ServerSocketHandler* socketHandler)
{
    User* user = socketHandler->GetUser();
    if (!user)
    {
        std::cout << "ERROR: Received text message from unauthorized user." << std::endl;
        socketHandler->Send("ERROR|NOT_AUTHORIZED;\r\n");
        return;
    }

    std::cout << "TEXT from [" << user->nickname << "] : " << message.text << std::endl;
    socketHandler->Send("RECEIVED|" + std::to_string(message.text.size()) + ";\r\n");
}
