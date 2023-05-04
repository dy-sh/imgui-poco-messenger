// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <Poco/FIFOBuffer.h>

enum class RawMessageType
{
    Connect = 49,
    TextMessage = 50,
};


struct RawMessage
{
    RawMessageType type;

    Poco::FIFOBuffer& buffer;
    size_t from;
    size_t size;


    RawMessage(Poco::FIFOBuffer& buffer): buffer{buffer}
    {
    }


    std::string MessageTypeToStr(RawMessageType type) const
    {
        switch (type)
        {
        case RawMessageType::Connect:
            return "Connect";
        case RawMessageType::TextMessage:
            return "TextMessage";
        default:
            return "Unknown";
        }
    }

    std::string to_str() const
    {
        return MessageTypeToStr(type) + ": " + std::string(buffer.begin() + from, size);
    }
};
