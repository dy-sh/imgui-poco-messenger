// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "../Message.h"
#include "../SimpleProtocol.h"

struct ServerAuthorizeMessage : Message
{
    static const std::string type;
    static const char prefix;

    int user_id = 0;
    std::string user_name;


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

            for (size_t x = parsing_from; x <= from + size; ++x)
            {
                if (buffer[x] == '|' || x == from + size)
                {
                    if (parsing_part == 0)
                    {
                        std::string num = std::string(buffer + parsing_from, x - parsing_from);
                        user_id = std::stoi(num);
                    }
                    else if (parsing_part == 1)
                    {
                        user_name = std::string(buffer + parsing_from, x - parsing_from - 1);
                    }
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


    static std::string Serialize(int user_id, std::string user_name)
    {
        return std::string(1, prefix) + "|" + std::to_string(user_id) + "|" + user_name + SimpleProtocol::DELIMITER +
            "\r\n";
    }


    std::string to_str() const override
    {
        return "AUTHORIZED: id: " + std::to_string(user_id) + " username: " + user_name;
    }
};
