#include "SimpleProtocol.h"
#include "RawMessage.h"


bool SimpleProtocol::parseMessage(RawMessage& outMessage)
{
    outMessage.from = 0;

    // find start (skip all '\r' and '\n')
    for (size_t i = 0; i < outMessage.buffer.used(); i++)
    {
        if (outMessage.buffer[i] == '\r' || outMessage.buffer[i] == '\n')
        {
            outMessage.from++;
        }
        else
        {
            break;
        }
    }

    // find end
    for (size_t i = outMessage.from; i < outMessage.buffer.used(); i++)
    {
        if (outMessage.buffer[i] == delimiter)
        {
            outMessage.type = (RawMessageType)outMessage.buffer[outMessage.from];
            outMessage.from++;
            outMessage.size = i + 1 - outMessage.from;
            return true;
        }
    }
    return false;
}
