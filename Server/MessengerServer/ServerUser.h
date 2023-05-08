// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>
#include <vector>

class ServerSocketHandler;

struct ServerUser
{
    size_t id;
    std::string nickname;
    std::vector<ServerSocketHandler*> socket_handlers;

    bool IsAuthorized() const { return id > 0; }
};
