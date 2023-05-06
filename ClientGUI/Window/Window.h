// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <imgui.h>
#include <string>

class Window
{
public:
    virtual ~Window() = default;


    Window(const std::string& title, bool visible = true, bool resizable = true, bool modal = false)
        : m_title(title), m_visible(visible), m_resizable(resizable), m_modal(modal)
    {
    }


    virtual void render()
    {
        if (m_visible)
        {
            ImGui::Begin(m_title.c_str(), &m_visible, m_resizable ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoResize);
            ImGui::End();
        }
    }


    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

    bool isResizable() const { return m_resizable; }
    void setResizable(bool resizable) { m_resizable = resizable; }

    bool isModal() const { return m_modal; }
    void setModal(bool modal) { m_modal = modal; }


    std::string getTitle() { return m_title; }

protected:
    std::string m_title;
    bool m_visible;
    bool m_resizable;
    bool m_modal;
};

class ModalWindow : public Window
{
public:
    ModalWindow(const std::string& title) : Window(title, true, false, true)
    {
    }


    virtual void RenderContent()
    {
    }


    void render() override
    {
        if (m_visible)
        {
            ImGui::OpenPopup(m_title.c_str());
            if (ImGui::BeginPopupModal(m_title.c_str(), &m_visible))
            {
                RenderContent();

                ImGui::EndPopup();
            }
        }
    }
};

class ResizableWindow : public Window
{
public:
    ResizableWindow(const std::string& title) : Window(title, true, true, false)
    {
    }


    virtual void RenderContent()
    {
    }


    void render() override
    {
        if (m_visible)
        {
            ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(FLT_MAX, FLT_MAX));
            ImGui::Begin(m_title.c_str(), &m_visible, ImGuiWindowFlags_None);

            RenderContent();

            ImGui::End();
        }
    }
};

