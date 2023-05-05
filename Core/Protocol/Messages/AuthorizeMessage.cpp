// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "AuthorizeMessage.h"


bool AuthorizeMessage::parse(const char* buffer, size_t from, size_t size)
{
    userName = std::string(buffer + from + 1, size - 2);
    return true;
}


std::string AuthorizeMessage::to_str() const
{
    return "CONNECT: " + userName;
}
