// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>
#include <vector>
#include <Poco/FIFOBuffer.h>

struct IProtocol
{
    virtual ~IProtocol() = default;
    virtual std::vector<std::string> parse(Poco::FIFOBuffer& buffer) =0;
};
