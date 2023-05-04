#include "SimpleProtocol.h"
#include "../Messages/RawMessage.h"


bool SimpleProtocol::parseMessage(RawMessage& outMessage)
{
    for (size_t i = 0; i < outMessage.buffer.used(); i++)
    {
        if (outMessage.buffer[i] == delimiter)
        {
            outMessage.type = (MessageType)outMessage.buffer[0];
            outMessage.from = 0;
            outMessage.size = i + 1;
            return true;
        }
    }
    return false;
}
