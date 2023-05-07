// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <imgui.h>
#include <string>

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


    virtual void Render()
    {
        if (visible)
        {
            ImGui::SetNextWindowSizeConstraints(size, ImVec2(FLT_MAX, FLT_MAX));
            // ImGui::SetNextWindowSize( size, ImGuiCond_FirstUseEver );

            ImGuiWindowFlags flags = 0;
            if (!options.resizable)
                flags = flags | ImGuiWindowFlags_NoResize;
            if (options.no_bring_to_front)
                flags = flags | ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (options.no_decoration)
                flags = flags | ImGuiWindowFlags_NoDecoration;


            ImGui::Begin(title.c_str(), &visible, flags);
            RenderContent();
            ImGui::End();
        }
    }


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


    void Render() override
    {
        if (visible)
        {
            ImGui::SetNextWindowSizeConstraints(size, ImVec2(FLT_MAX, FLT_MAX));
            // ImGui::SetNextWindowSize( size, ImGuiCond_FirstUseEver );

            // // centered on screen
            // SetWindowPosition();
            // ImGui::Begin(title.c_str(), &visible,
            //              ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            // RenderContent();
            // ImGui::End();

            ImGui::OpenPopup(title.c_str());
            if (ImGui::BeginPopupModal(title.c_str(), &visible))
            {
                RenderContent();

                ImGui::EndPopup();
            }
        }
    }


    bool IsModal() const override { return true; }


    void SetWindowPosition()
    {
        ImGui::SetNextWindowSize(size);

        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 window_center(window_size.x / 2, window_size.y / 2);

        ImGui::SetNextWindowPos(
            {window_center.x + (size.x / 2), window_center.y + (size.y / 2)},
            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    }
};

class MaximizedWindow : public Window
{
public:
    MaximizedWindow(const std::string& title, bool visible = true)
        : Window(title, visible)
    {
    }


    void Render() override
    {
        if (visible)
        {
            MaximizeMainWindow();

            ImGui::Begin(title.c_str(), &visible, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoBringToFrontOnFocus);

            RenderContent();

            ImGui::End();

            FinishMaximizeMainWindow();
        }
    }

protected:
    float OriginalWindowRounding;


    virtual void MaximizeMainWindow()
    {
#ifdef IMGUI_HAS_VIEWPORT
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );
#else
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif

        ImGuiStyle* style = &ImGui::GetStyle();
        OriginalWindowRounding = style->WindowRounding;
        style->WindowRounding = 0.0;
    }


    virtual void FinishMaximizeMainWindow()
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        style->WindowRounding = OriginalWindowRounding;
    }
};
