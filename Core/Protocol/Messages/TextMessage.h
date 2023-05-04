// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

struct TextMessage
{
    std::string userName;
    std::string text;

    TextMessage(RawMessage message)
    {
        text = std::string(message.buffer.begin() + message.from, message.size - 1);
    }


    std::string to_str() const
    {
        return "TEXT: " + text;
    }
};
