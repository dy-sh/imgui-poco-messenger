// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <memory>

#include "Messages/Message.h"


struct IProtocol
{
    virtual ~IProtocol() = default;
    virtual std::pair<std::unique_ptr<Message>, size_t> ParseMessage(const char* buffer, size_t buffer_size)=0;
};
