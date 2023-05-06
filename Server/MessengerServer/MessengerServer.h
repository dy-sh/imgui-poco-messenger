﻿// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <memory>
#include <vector>

struct Message;
struct TextMessage;
struct AuthorizeMessage;
struct ServerUser;
class ServerSocketHandler;

class MessengerServer
{
public:
    std::vector<ServerUser*> getAllAuthorizedUsers();
    
    void receiveMessage(Message* message, ServerSocketHandler* socketHandler);
private:
    size_t last_user_id{0};
    std::vector<ServerUser*> users;
    // std::vector<TextMessage> messages;
    void authorizeUser(AuthorizeMessage& message, ServerSocketHandler* socketHandler);
    void receiveText(TextMessage& message, ServerSocketHandler* socketHandler);
};
