// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

#include "Message.h"

struct ClientAuthorizeMessage : Message
{
    static const std::string type;

    std::string user_name;

    ClientAuthorizeMessage(char prefix) : Message(prefix)
    {
    }

    bool Parse(const char* buffer, size_t from, size_t size) override;

    std::string to_str() const override;
};
