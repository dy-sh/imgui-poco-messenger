// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "IProtocol.h"


constexpr char DELIMITER = ';';

struct SimpleProtocol : public IProtocol
{
    std::pair<std::unique_ptr<Message>, size_t> parseMessage(const char* buffer, size_t buffer_size) override;
};
