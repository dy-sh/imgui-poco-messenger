// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once


class ServerSocketHandler;
struct RawMessage;

class Messenger
{
    // std::vector<User> users;
    // std::vector<TextMessage> messages;
public:
    void receiveMessage(RawMessage& message, ServerSocketHandler* socketHandler);
};
