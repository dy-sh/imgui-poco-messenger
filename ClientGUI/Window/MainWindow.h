//
// Created by Dmitry Savosh on 18.04.2023.
//

#pragma once
#include "Window.h"


class Client;

class MainWindow : public MaximizedWindow
{
public:
    MainWindow(const std::string& title, bool visible = true) :MaximizedWindow(title, visible ){  }
    void RenderContent() override;

private:
    Client* client;  
   
};
