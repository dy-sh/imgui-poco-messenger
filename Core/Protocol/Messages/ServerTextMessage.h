// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "Message.h"

struct ServerTextMessage : Message
{
    static const std::string type;

    int user_id = 0;
    std::string user_name;
    std::string text;


    ServerTextMessage(char prefix) : Message(prefix)
    {
    }


    bool Parse(const char* buffer, size_t from, size_t size) override
    {
        int parsing_part = 0;
        size_t parsing_from = from + 2; // skip header with message type

        for (size_t x = parsing_from; x <= size; ++x)
        {
            if (buffer[x] == '|' || x == size)
            {
                if (parsing_part == 0)
                {
                    std::string num = std::string(buffer + parsing_from, x - parsing_from);
                    user_id = std::stoi(num);
                }
                else if (parsing_part == 1)
                {
                    user_name = std::string(buffer + parsing_from, x - parsing_from);
                }
                else if (parsing_part == 2)
                {
                    text = std::string(buffer + parsing_from, x - parsing_from + 1);
                }
                parsing_from = x + 1;
                parsing_part++;
            }
        }

        return parsing_part = 3; // validate all parts parsed
    }

    std::string to_str() const override
    {
        return user_name + ": " + text;
    }
};
