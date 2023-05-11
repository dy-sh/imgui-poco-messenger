// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <vector>

#include "../Message.h"
#include "../SimpleProtocol.h"

struct ServerRoomListMessage : Message
{
    static const std::string type;
    static const char prefix;

    std::vector<int> room_ids;
    std::vector<std::string> room_names;


    bool Matches(const char* buffer, size_t from, size_t size) const override
    {
        // check message starts from type and |, example "A|...."
        return buffer[from] == prefix && buffer[from + 1] == '|';
    }


    bool Deserialize(const char* buffer, size_t from, size_t size) override
    {
        try
        {
            int parsing_part = 0;
            size_t parsing_from = from + 2; // skip header with message type

            for (size_t x = parsing_from; x <= from + size - 1; ++x)
            {
                if (buffer[x] == '|' || x == from + size - 1)
                {
                    if (parsing_part == 0)
                    {
                        std::string s(buffer + parsing_from, x - parsing_from);
                        room_ids = DESERIALIZE_VECTOR_INT(s);
                    }
                    if (parsing_part == 1)
                    {
                        std::string s(buffer + parsing_from, x - parsing_from);
                        room_names = DESERIALIZE_VECTOR_STR(s);
                    }

                    parsing_from = x + 1;
                    parsing_part++;
                }
            }

            return parsing_part = 2; // validate all parts parsed
        }
        catch (...)
        {
            return false;
        }
    }


    static std::string Serialize(std::vector<int> room_ids, std::vector<std::string> room_names)
    {
        std::string ids = SERIALIZE_VECTOR_INT(room_ids);
        std::string names = SERIALIZE_VECTOR_STR(room_names);
        return std::string(1, prefix)
            + "|" + ids
            + "|" + names
            + SimpleProtocol::DELIMITER + "\r\n";
    }


    std::string to_str() const override
    {
        return "ROOMS: " + std::to_string(room_ids.size()) ;
    }
};
