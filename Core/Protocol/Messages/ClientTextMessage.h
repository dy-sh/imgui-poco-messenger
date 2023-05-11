// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

#include "../Message.h"
#include "../SimpleProtocol.h"

struct ClientTextMessage : Message
{
    static const std::string type;
    static const char prefix;

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
            text = std::string(buffer + from + 2, size - 3);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }


    static std::string Serialize(std::string text)
    {
        return std::string(1, prefix) + "|" + text + SimpleProtocol::DELIMITER + "\r\n";
    }


    std::string to_str() const override
    {
        return "TEXT: " + text;
    }
};
