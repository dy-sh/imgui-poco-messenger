// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <Poco/FIFOBuffer.h>

enum class MessageType
{
    Connect,
    TextMessage
};

struct RawMessage
{
    MessageType type;

    Poco::FIFOBuffer& buffer;
    size_t from;
    size_t size;


    RawMessage(Poco::FIFOBuffer& buffer): buffer{buffer}
    {
    }


    std::string to_str() const
    {
        return std::string(buffer.begin() + from, size);
    }
};
