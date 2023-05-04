// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>
#include <vector>

#include "Messages/TextMessage.h"
#include "User/User.h"

class Messenger
{
    std::vector<User> users;
    std::vector<TextMessage> messages;
public:
    void receiveMessages(std::vector<std::string> messages);
};
