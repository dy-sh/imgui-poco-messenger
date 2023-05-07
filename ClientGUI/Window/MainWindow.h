//
// Created by Dmitry Savosh on 18.04.2023.
//

#pragma once
#include "Window.h"


class WindowManager;
class Client;

class MainWindow : public MaximizedWindow 
{
public:
    MainWindow(const std::string& title, WindowManager* windowManager, bool visible = true)
        : MaximizedWindow(title, visible), windowManager(windowManager)
    {
    }


    void RenderContent() override;

    void OnExit() override;

private:
    Client* client = nullptr;
    WindowManager* windowManager = nullptr;
    char server_address[100] = "127.0.0.1";
    int server_port = 9977;
};
