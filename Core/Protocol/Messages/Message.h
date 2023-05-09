// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>


struct Message
{
    Message(char prefix) : prefix(prefix)
    {
    }


    virtual ~Message()
    {
    }


    virtual bool Matches(const char* buffer, size_t from, size_t size) const
    {
        return buffer[0 + from] == prefix;
    }


    virtual bool Parse(const char* buffer, size_t from, size_t size){return false;}
    virtual std::string to_str() const{return "";} 

private:
    const char prefix;
};





