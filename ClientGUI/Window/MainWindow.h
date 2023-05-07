//
// Created by Dmitry Savosh on 18.04.2023.
//

#pragma once
#include "Window.h"


class WindowManager;
class Client;

class MainWindow : public Window // or MaximizedWindow
{
public:
    MainWindow(const std::string& title, WindowManager* windowManager, bool visible = true)
        : Window(title, visible,{0,0},{true,true}), windowManager(windowManager)
    {
    }


    void RenderContent() override;

private:
    Client* client;
    WindowManager* windowManager;
};
