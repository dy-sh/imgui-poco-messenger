// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "Window.h"


class WindowManager;
class Client;

class MainWindow : public MaximizedWindow
{
public:
    MainWindow(const std::string& title, bool visible, WindowManager* windowManager, Client* client);


    void RenderContent() override;

    void OnExit() override;

private:
    Client* client = nullptr;
    WindowManager* windowManager = nullptr;
    char server_address[100] = "127.0.0.1";
    int server_port = 9977;
};
