#include "Messenger.h"
#include "ServerSocketHandler.h"
#include "Messages/RawMessage.h"





void Messenger::receiveMessage(RawMessage& message, ServerSocketHandler* socketHandler)
{
    std::cout << "Received message: " << message.to_str() << std::endl;
}
