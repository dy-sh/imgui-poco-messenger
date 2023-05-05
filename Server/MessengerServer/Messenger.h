// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <vector>

struct TextMessage;
struct ConnectMessage;
struct User;
class ServerSocketHandler;
struct RawMessage;

class Messenger
{
public:
    void receiveMessage(RawMessage& message, ServerSocketHandler* socketHandler);

private:
    size_t last_user_id{0};
    std::vector<User*> users;
    // std::vector<TextMessage> messages;
    void connectUser(ConnectMessage& message, ServerSocketHandler* socketHandler);
    void receiveText(TextMessage& message, ServerSocketHandler* socketHandler);
};
