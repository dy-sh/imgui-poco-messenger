﻿#include "WindowManager.h"


void WindowManager::Render()
{
    for (auto& window : windows)
    {
        if (!window->IsModal())
        {
            window->Render();
        }
    }

    for (auto& window : windows)
    {
        if (window->IsModal())
        {
            window->Render();
        }
    }
}


void WindowManager::AddWindow(std::unique_ptr<Window> window)
{
    windows.push_back(std::move(window));
}


void WindowManager::RemoveWindow(Window* window)
{
    windows.erase(std::remove_if(windows.begin(), windows.end(),
                                 [window](const auto& w) { return w.get() == window; }), windows.end());
}


Window* WindowManager::GetWindowByTitle(const std::string& title)
{
    for (auto& window : windows)
    {
        if (window->GetTitle() == title)
        {
            return window.get();
        }
    }
    return nullptr;
}
