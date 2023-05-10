// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <set>
#include <string>

class ServerSocketHandler;

struct ServerUser
{
    size_t id;
    std::string user_name;
    std::set<ServerSocketHandler*> sockets;

    bool IsAuthorized() const { return id > 0; }
};
