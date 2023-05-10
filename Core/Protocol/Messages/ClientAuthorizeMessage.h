// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

#include "Message.h"
#include "../SimpleProtocol.h"

struct ClientAuthorizeMessage : Message
{
    static const std::string type;
    static const char prefix;

    std::string user_name;


    ClientAuthorizeMessage() : Message()
    {
    }

    bool Matches(const char* buffer, size_t from, size_t size) const override
    {
        // check message starts from type and |, example "A|...."
        return buffer[from] == prefix && buffer[from + 1] == '|';
    }

    bool Deserialize(const char* buffer, size_t from, size_t size) override
    {
        try
        {
            user_name = std::string(buffer + from + 2, size - 3);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }


    static std::string Serialize(std::string user_name) 
    {
        return std::string(1, prefix) + "|" + user_name + SimpleProtocol::DELIMITER;
    }


    std::string to_str() const override
    {
        return "AUTHORIZE: " + user_name;
    }
};
