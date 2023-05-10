// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "../Window.h"


struct Message;
class WindowManager;
class Client;

class ConnectionWindow : public MaximizedWindow
{
public:
    ConnectionWindow(const std::string& title, bool visible, WindowManager* window_manager, Client* client);
    ~ConnectionWindow();

    void RenderContent() override;
    void OnExit() override;

    void OnReceiveMessage(const void* sender, Message*& message);
    void OnConnected(const void* sender);
    void OnDisconnected(const void* sender);

private:
    Client* client = nullptr;
    WindowManager* window_manager = nullptr;
    char server_address[100] = "127.0.0.1";
    char user_name[32] = "User1";
    int server_port = 9977;
    bool authorized = false;
};
