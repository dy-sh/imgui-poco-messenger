#include "Messenger.h"
#include "ServerSocketHandler.h"
#include "Protocol/RawMessage.h"
#include "Protocol/Messages/ConnectMessage.h"
#include "Protocol/Messages/TextMessage.h"


void Messenger::receiveMessage(RawMessage& message, ServerSocketHandler* socketHandler)
{
    if (message.type==RawMessageType::Connect)
    {
        ConnectMessage m(message);
        std::cout << "Received message: " << m.to_str() << std::endl;
    }
    else if (message.type==RawMessageType::TextMessage)
    {
        TextMessage m(message);
        std::cout << "Received message: " << m.to_str() << std::endl;
    }
    else
    {
        std::cout << "Received message: " << message.to_str() << std::endl;
    }
}
