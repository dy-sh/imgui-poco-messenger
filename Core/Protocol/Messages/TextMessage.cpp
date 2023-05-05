// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "TextMessage.h"


bool TextMessage::parse(const char* buffer, size_t from, size_t size)
{
    text = std::string(buffer + from + 1, size - 2);
    return true;
}


std::string TextMessage::to_str() const
{
    return "TEXT: " + text;
}
