// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "../Message.h"
#include "../SimpleProtocol.h"

struct ServerTextMessage : Message
{
    static const std::string type;
    static const char prefix;

    int user_id = 0;
    int room_id = 0;
    std::string user_name;
    std::string text;


    bool Matches(const char* buffer, size_t from, size_t size) const override
    {
        // check message starts from type and |, example "A|...."
        return buffer[from] == prefix && buffer[from + 1] == '|';
    }


    bool Deserialize(const char* buffer, size_t from, size_t size) override
    {
        try
        {
            int parsing_part = 0;
            size_t parsing_from = from + 2; // skip header with message type

            for (size_t x = parsing_from; x <= from + size - 1; ++x)
            {
                if (buffer[x] == '|' || x == from + size - 1)
                {
                    PARSE_INT(user_id, 0)
                    PARSE_INT(room_id, 1)
                    PARSE_STRING(user_name, 2)
                    PARSE_STRING(text, 3)

                    parsing_from = x + 1;
                    parsing_part++;
                }
            }

            return parsing_part = 4; // validate all parts parsed
        }
        catch (...)
        {
            return false;
        }
    }


    static std::string Serialize(int user_id, int room_id, std::string user_name, std::string text)
    {
        return std::string(1, prefix)
            + "|" + std::to_string(user_id)
            + "|" + std::to_string(room_id)
            + "|" + user_name
            + "|" + text
            + SimpleProtocol::DELIMITER + "\r\n";
    }


    std::string to_str() const override
    {
        return user_name + ": " + text;
    }
};
