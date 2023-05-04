// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include <sstream>
#include <Poco/FIFOBuffer.h>
#include <string>
#include <vector>

#include "IProtocol.h"

using Poco::FIFOBuffer;

class MessengerSimpleProtocol:public IProtocol
{
public:
    const char delimiter = ';';

    std::vector<std::string> parse(FIFOBuffer& buffer) override;
};
