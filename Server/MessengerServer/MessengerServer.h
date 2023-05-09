// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <vector>

struct Message;
struct ClientTextMessage;
struct ClientAuthorizeMessage;
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
    void AuthorizeUser(ClientAuthorizeMessage& message, ServerSocketHandler* socketHandler);
    void ReceiveText(ClientTextMessage& message, ServerSocketHandler* socketHandler);
};
