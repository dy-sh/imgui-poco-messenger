// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <memory>
#include <vector>

struct Message;
struct TextMessage;
struct AuthorizeMessage;
struct User;
class ClientSocketHandler;

class MessengerClient
{
public:
    std::vector<User*> getAllAuthorizedUsers();
    
    void receiveMessage(Message* message, ClientSocketHandler* socketHandler);
private:
    size_t last_user_id{0};
    std::vector<User*> users;
    // std::vector<TextMessage> messages;
    void authorizeUser(AuthorizeMessage& message, ClientSocketHandler* socketHandler);
    void receiveText(TextMessage& message, ClientSocketHandler* socketHandler);
};
