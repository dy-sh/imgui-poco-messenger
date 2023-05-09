// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

#include "Message.h"

struct ClientTextMessage : Message
{
    static const std::string type;

    std::string text;

    ClientTextMessage(char prefix) : Message(prefix)
    {
    }


    bool Parse(const char* buffer, size_t from, size_t size) override
    {
        text = std::string(buffer + from + 1, size - 2);
        return true;
    }

    std::string to_str() const override
    {
        return "TEXT: " + text;
    }
};

