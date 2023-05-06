// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>
#include <vector>

class ClientSocketHandler;

struct ClientUser
{
    size_t id;
    std::string nickname;
    std::vector<ClientSocketHandler*> socketHandlers;

    bool IsAuthorized() const { return id > 0; }
};
