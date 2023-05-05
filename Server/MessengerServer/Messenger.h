// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <memory>
#include <vector>

struct Message;
struct TextMessage;
struct AuthorizeMessage;
struct User;
class ServerSocketHandler;

class Messenger
{
public:
    void receiveMessage(Message* message, ServerSocketHandler* socketHandler);

private:
    size_t last_user_id{0};
    std::vector<User*> users;
    // std::vector<TextMessage> messages;
    void connectUser(AuthorizeMessage& message, ServerSocketHandler* socketHandler);
    void receiveText(TextMessage& message, ServerSocketHandler* socketHandler);
};
