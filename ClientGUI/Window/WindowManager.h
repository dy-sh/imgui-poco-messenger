// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <vector>
#include <string>
#include <imgui.h>
#include <memory>

#include "Window.h"


class WindowManager
{
public:
    void render()
    {
        for (auto& window : m_windows)
        {
            if (window->isModal())
            {
                window->render();
            }
        }

        for (auto& window : m_windows)
        {
            if (!window->isModal())
            {
                window->render();
            }
        }
    }


    void addWindow(std::unique_ptr<Window> window)
    {
        m_windows.push_back(std::move(window));
    }


    void removeWindow(Window* window)
    {
        m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(),
                                       [window](const auto& w) { return w.get() == window; }), m_windows.end());
    }


    Window* getWindowByTitle(const std::string& title)
    {
        for (auto& window : m_windows)
        {
            if (window->getTitle() == title)
            {
                return window.get();
            }
        }
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<Window>> m_windows;
};
