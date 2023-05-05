// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "InvalidMessage.h"


InvalidMessage::InvalidMessage(const char* buffer, size_t from, size_t size): Message('I')
{
    text = std::string(buffer + from , size - 1);
}


bool InvalidMessage::parse(const char* buffer, size_t from, size_t size)
{
    text = std::string(buffer + from , size - 1);
    return true;
}


std::string InvalidMessage::to_str() const
{
    return "Invalid message: " + text;
}