// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include <sstream>
#include <Poco/FIFOBuffer.h>
#include <string>
#include <vector>

#include "IProtocol.h"


class SimpleProtocol:public IProtocol
{
public:
    const char delimiter = ';';

    bool SimpleProtocol::parseMessage(RawMessage& outMessage);
};
