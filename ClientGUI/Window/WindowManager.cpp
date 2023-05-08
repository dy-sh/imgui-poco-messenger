// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "WindowManager.h"
#include <algorithm>

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


void WindowManager::OnExit()
{
    for (auto& window : windows)
    {
        window->OnExit();
    }
}
