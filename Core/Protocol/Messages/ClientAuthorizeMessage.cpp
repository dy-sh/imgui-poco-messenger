// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientAuthorizeMessage.h"


bool ClientAuthorizeMessage::Parse(const char* buffer, size_t from, size_t size)
{
    user_name = std::string(buffer + from + 1, size - 2);
    return true;
}


std::string ClientAuthorizeMessage::to_str() const
{
    return "AUTHORIZE: " + user_name;
}
