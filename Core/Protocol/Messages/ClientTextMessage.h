// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

#include "../Message.h"
#include "../SimpleProtocol.h"

struct ClientTextMessage : Message
{
    static const std::string type;
    static const char prefix;

    int room_id;
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
                    PARSE_INT(room_id, 0)
                    PARSE_STRING(text, 1)

                    parsing_from = x + 1;
                    parsing_part++;
                }
            }

            return parsing_part = 2; // validate all parts parsed
        }
        catch (...)
        {
            return false;
        }
    }


    static std::string Serialize(int room_id, std::string text)
    {
        return std::string(1, prefix)
            + "|" + std::to_string(room_id)
            + "|" + text
            + SimpleProtocol::DELIMITER + "\r\n";
    }


    std::string to_str() const override
    {
        return "TEXT: " + text;
    }
};
