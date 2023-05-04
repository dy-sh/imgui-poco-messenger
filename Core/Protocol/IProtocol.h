// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>
#include <vector>
#include <Poco/FIFOBuffer.h>

#include "../Messages/RawMessage.h"

struct IProtocol
{
    virtual ~IProtocol() = default;
    virtual bool parseMessage(RawMessage& outMessage)=0;
};
