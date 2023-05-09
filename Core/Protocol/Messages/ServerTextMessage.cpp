#include "ServerTextMessage.h"


bool ServerTextMessage::Parse(const char* buffer, size_t from, size_t size)
{
    size_t part_from = from + 1;
    for (size_t x = from + 1; x < size - 2; ++x)
    {
        if (buffer[x] == '|')
        {
            if (user_id == 0)
            {
                // user_id=
            }
            else if (user_name.empty())
            {
                user_name = std::string(buffer + part_from, x - part_from);
                part_from = x + 1;
            }
        }
        if (buffer[x] == ';')
        {
            text = std::string(buffer + part_from, x - part_from);
            part_from = x + 1;
        }
    }

    return true;
}


std::string ServerTextMessage::to_str() const
{
    return user_name + ": " + text;
}
