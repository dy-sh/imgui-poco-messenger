// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

struct User
{
    size_t id; 
    std::string nickname;
    std::vector<ServerSocketHandler*> socketHandlers;
};
