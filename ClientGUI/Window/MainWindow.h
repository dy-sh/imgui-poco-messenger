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

private:
    Client* client;
    WindowManager* windowManager;
};
