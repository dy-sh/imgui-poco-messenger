// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <vector>

struct Message;
struct TextMessage;
struct AuthorizeMessage;
struct ServerUser;
class ServerSocketHandler;

class MessengerServer
{
public:
    std::vector<ServerUser*> GetAllAuthorizedUsers();

    void ReceiveMessage(Message* message, ServerSocketHandler* socketHandler);

private:
    size_t last_user_id{0};
    std::vector<ServerUser*> users;
    // std::vector<TextMessage> messages;
    void AuthorizeUser(AuthorizeMessage& message, ServerSocketHandler* socketHandler);
    void ReceiveText(TextMessage& message, ServerSocketHandler* socketHandler);
};
