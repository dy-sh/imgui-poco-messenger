// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <vector>

struct Room
{
    std::vector<char*> items;
    bool new_messages = false;
    std::string name = "";
};
