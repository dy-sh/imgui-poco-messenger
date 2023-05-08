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
    void ReceiveText(const TextMessage& text_message, ClientSocketHandler* socket_handler);
    void ReceiveMessage(Message* message, ClientSocketHandler* socket_handler);
private:
    size_t user_id{0};
};
