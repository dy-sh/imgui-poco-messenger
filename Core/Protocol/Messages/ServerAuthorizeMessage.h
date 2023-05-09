// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "Message.h"

struct ServerAuthorizeMessage : Message
{
    static const std::string type;

    int user_id = 0;
    std::string user_name;


    ServerAuthorizeMessage(char prefix) : Message(prefix)
    {
    }


    bool Parse(const char* buffer, size_t from, size_t size) override
    {
        size_t parsing_from = from + 1;
        int parsing_part = 0;

        for (size_t x = from + 1; x <= size; ++x)
        {
            char c = buffer[x];
            if (buffer[x] == '|' || x == size)
            {
                if (parsing_part == 0)
                {
                    // user_id=
                }
                else if (parsing_part == 1)
                {
                    user_name = std::string(buffer + parsing_from, x - parsing_from + 1);
                }
                parsing_from = x + 1;
                parsing_part++;
            }
        }

        return parsing_part = 2; // validate all parts parsed
    }


    std::string to_str() const override
    {
        return "AUTHORIZED: id: " + std::to_string(user_id) + " username: " + user_name;
    }
};
