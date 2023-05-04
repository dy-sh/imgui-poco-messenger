// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

#include "../RawMessage.h"

struct ConnectMessage
{
    std::string userName;

    ConnectMessage(RawMessage message)
    {
        userName = std::string(message.buffer.begin() + message.from, message.size - 1);
    }


    std::string to_str() const
    {
        return "CONNECT: " + userName;
    }
};
