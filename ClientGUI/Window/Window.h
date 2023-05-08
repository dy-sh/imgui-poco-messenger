// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <string>
#include "imgui.h"

struct WindowOptions
{
    bool resizable = true;
    bool no_bring_to_front = false;
    bool no_decoration = false;
};

class Window
{
public:
    Window(const std::string& title, bool visible = true, ImVec2 size = {400, 500}, WindowOptions options = {})
        : title(title), visible(visible), options(options), size(size)
    {
    }
    
    virtual ~Window() = default;

    virtual void RenderContent()
    {
    }
    
    virtual void Render();

    bool IsVisible() const { return visible; }
    void SetVisible(bool vis) { visible = vis; }
    void ToggleVisible() { visible = !visible; }

    bool IsResizable() const { return options.resizable; }
    void SetResizable(bool res) { options.resizable = res; }

    virtual bool IsModal() const { return false; }

    void SetTitle(std::string title) { this->title = title; }
    std::string GetTitle() { return title; }

    void SetOptions(WindowOptions options) { this->options = options; }
    WindowOptions GetOptions() { return options; }


    virtual void OnExit()
    {
    }

protected:
    std::string title;
    bool visible;
    ImVec2 size;
    WindowOptions options;
};

class ModalWindow : public Window
{
public:
    ModalWindow(const std::string& title, bool visible = true, ImVec2 size = {400, 500})
        : Window(title, visible, size)
    {
    }

    void Render() override;
    bool IsModal() const override { return true; }
    void SetWindowPosition();
};

class MaximizedWindow : public Window
{
public:
    MaximizedWindow(const std::string& title, bool visible = true)
        : Window(title, visible)
    {
    }

    void Render() override;

protected:
    float OriginalWindowRounding;

    virtual void MaximizeMainWindow();
    virtual void FinishMaximizeMainWindow();
};
