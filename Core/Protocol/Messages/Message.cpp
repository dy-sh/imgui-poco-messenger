// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Message.h"

bool Message::Matches(const char* buffer, size_t from, size_t size) const
{
    return buffer[0 + from] == prefix;
}
