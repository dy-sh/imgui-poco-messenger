// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <imgui.h>
#include <string>

class Window
{
public:
    Window(const std::string& title, bool visible = true, bool resizable = true, ImVec2 size = {400, 500})
        : title(title), visible(visible), resizable(resizable), size(size)
    {
        resizable = true;
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
            // ImGui::SetNextWindowSize( ImVec2( 400, 500 ), ImGuiCond_FirstUseEver );
            ImGui::Begin(title.c_str(), &visible, resizable ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoResize);
            RenderContent();
            ImGui::End();
        }
    }


    bool isVisible() const { return visible; }
    void setVisible(bool visible) { visible = visible; }

    bool isResizable() const { return resizable; }
    void setResizable(bool resizable) { resizable = resizable; }

    virtual bool isModal() const { return false; }

    void setTitle(std::string title) { this->title = title; }
    std::string getTitle() { return title; }

protected:
    std::string title;
    bool visible;
    bool resizable;
    ImVec2 size;
};

class ModalWindow : public Window
{
public:
    ModalWindow(const std::string& title, bool visible = true, ImVec2 size = {400, 500})
        : Window(title, visible, false, size)
    {
    }


    void Render() override
    {
        if (visible)
        {
            ImGui::OpenPopup(title.c_str());
            if (ImGui::BeginPopupModal(title.c_str(), &visible))
            {
                RenderContent();

                ImGui::EndPopup();
            }
        }
    }

    bool isModal() const override { return true; }
};

class MaximizedWindow : public Window
{
public:
    MaximizedWindow(const std::string& title, bool visible = true)
        : Window(title, visible, false, {400, 500})
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
