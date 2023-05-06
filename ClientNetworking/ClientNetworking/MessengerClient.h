// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <memory>
#include <vector>

struct Message;
struct TextMessage;
struct AuthorizeMessage;
struct ClientUser;
class ClientSocketHandler;

class MessengerClient
{
public:
    std::vector<ClientUser*> getAllAuthorizedUsers();
    
    void receiveMessage(Message* message, ClientSocketHandler* socketHandler);
private:
    size_t last_user_id{0};
    std::vector<ClientUser*> users;
    // std::vector<TextMessage> messages;
    void authorizeUser(AuthorizeMessage& message, ClientSocketHandler* socketHandler);
    void receiveText(TextMessage& message, ClientSocketHandler* socketHandler);
};
