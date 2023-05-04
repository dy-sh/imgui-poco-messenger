#include "SimpleProtocol.h"


std::vector<std::string> SimpleProtocol::parse(FIFOBuffer& buffer)
{
    std::vector<std::string> messages;

    std::stringstream ss;
    bool parsing = true;
    while (parsing)
    {
        parsing = false;
        for (int i = 0; i < buffer.used(); i++)
        {
            if (buffer[i] == delimiter)
            {
                std::string message = ss.str();
                size_t size = message.size() + 1;
                buffer.drain(size);
                ss.str(""); // clear stringstream
                // std::cout << "Received message: " << message << std::endl;
                messages.push_back(message);
                parsing = true;
                break;
            }
            else
            {
                ss << buffer[i];
            }
        }
    }

    return messages;
}
