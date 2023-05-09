// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

#include "Message.h"

struct InvalidMessage : Message
{
    static const std::string type;

    std::string text;


    InvalidMessage(char prefix) : Message(prefix)
    {
    }


    InvalidMessage(const char* buffer, size_t from, size_t size): Message('I')
    {
        text = std::string(buffer + from, size - 1);
    }


    bool Parse(const char* buffer, size_t from, size_t size) override
    {
        try
        {
            text = std::string(buffer + from, size - 1);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }


    std::string to_str() const override
    {
        return "Invalid message: " + text;
    }
};
