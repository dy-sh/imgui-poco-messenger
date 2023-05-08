// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Window.h"


class WindowManager
{
public:
    void Render();
    
    void AddWindow(std::unique_ptr<Window> window);
    
    void RemoveWindow(Window* window);
    
    Window* GetWindowByTitle(const std::string& title);
    
    void OnExit();

private:
    std::vector<std::unique_ptr<Window>> windows;
};
