// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>

struct ServerRoom
{
    int id;
    std::string name;


    ServerRoom(int id, const std::string& name)
        : id(id),
          name(name)
    {
    }
};
