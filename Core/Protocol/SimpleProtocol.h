﻿// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include <vector>

#include "IProtocol.h"

// This is simple text protocol for client-server communication
// Examples:
// Auser1; - authotize "user1"
// Aguest; - authotize "guest"
// Thello; - send text message "hello"
// Auser1;Thello; - authotize "user1" and say "hello"
// 
// To avoid problems with garbage in communications that terminal programs can send,
// it is better to start and end a message with a ";'
// Example:
// ;Auser1;Thello; - authotize "user1" and say "hello"
// ;;;;Auser1;;;Thello; - also valid messages, ";" will be ignored


struct SimpleProtocol : public IProtocol
{
    inline static char DELIMITER = ';';

    std::pair<std::unique_ptr<Message>, size_t> ParseMessage(const char* buffer, size_t buffer_size) override;
};


#define PARSE_INT(VariableName, PartNum) \
if (parsing_part == PartNum) \
{ \
    std::string num = std::string(buffer + parsing_from, x - parsing_from); \
    VariableName = std::stoi(num); \
}

#define PARSE_STRING(VariableName, PartNum) \
if (parsing_part == PartNum) \
{ \
    VariableName = std::string(buffer + parsing_from, x - parsing_from ); \
}


static std::string SERIALIZE_VECTOR_STR(std::vector<std::string> vec)
{
    std::string res = "";

    for (auto& s : vec)
    {
        res.append(s + "/");
    }
    return res;
}


static std::vector<std::string> DESERIALIZE_VECTOR_STR(std::string str)
{
    std::vector<std::string> res;

    int from = 0;
    for (int x = 0; x < str.size(); x++)
    {
        if (str[x] == '/')
        {
            std::string s(str.c_str() + from, x - from);
            res.push_back(s);
            from = x + 1;
        }
    }
    return res;
}


static std::string SERIALIZE_VECTOR_INT(std::vector<int> vec)
{
    std::string res = "";

    for (auto& s : vec)
    {
        res.append(std::to_string(s) + "/");
    }
    return res;
}


static std::vector<int> DESERIALIZE_VECTOR_INT(std::string str)
{
    std::vector<int> res;

    int from = 0;
    for (int x = 0; x < str.size(); x++)
    {
        if (str[x] == '/')
        {
            std::string s(str.c_str() + from, x - from);
            int num = std::stoi(s);
            res.push_back(num);
            from = x + 1;
        }
    }
    return res;
}