// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "../Message.h"
#include "../SimpleProtocol.h"

struct ClientGetRoomsMessage: Message
{
    static const std::string type;
    static const char prefix;


    bool Matches(const char* buffer, size_t from, size_t size) const override
    {
        // check message starts from type and |, example "A|...."
        return buffer[from] == prefix && buffer[from + 1] == '|';
    }


    bool Deserialize(const char* buffer, size_t from, size_t size) override
    {

            return true;

    }


    static std::string Serialize()
    {
        return std::string(1, prefix)
        + "|" + SimpleProtocol::DELIMITER + "\r\n";
    }


    std::string to_str() const override
    {
        return "GET ROOMS";
    }
};