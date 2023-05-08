// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessengerClient.h"
#include "ClientSocketHandler.h"
#include "Protocol/Messages/AuthorizeMessage.h"
#include "Protocol/Messages/InvalidMessage.h"
#include "Protocol/Messages/TextMessage.h"
#include "ClientUser.h"


void MessengerClient::ReceiveText(const TextMessage& text_message, ClientSocketHandler* socket_handler)
{
}


void MessengerClient::ReceiveMessage(Message* message, ClientSocketHandler* socket_handler)
{
    if (auto textMess = dynamic_cast<TextMessage*>(message))
    {
        ReceiveText(*textMess, socket_handler);
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


