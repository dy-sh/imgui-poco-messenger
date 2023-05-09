// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientTextMessage.h"


bool ClientTextMessage::Parse(const char* buffer, size_t from, size_t size)
{
    text = std::string(buffer + from + 1, size - 2);
    return true;
}


std::string ClientTextMessage::to_str() const
{
    return "TEXT: " + text;
}
