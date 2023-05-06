// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>
#include <vector>

class ServerSocketHandler;

struct ServerUser
{
    size_t id;
    std::string nickname;
    std::vector<ServerSocketHandler*> socketHandlers;

    bool IsAuthorized() const { return id > 0; }
};
