// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Window.h"


void Window::Render()
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


void ModalWindow::Render()
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


void ModalWindow::SetWindowPosition()
{
    ImGui::SetNextWindowSize(size);

    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 window_center(window_size.x / 2, window_size.y / 2);

    ImGui::SetNextWindowPos(
        {window_center.x + (size.x / 2), window_center.y + (size.y / 2)},
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
}


void MaximizedWindow::Render()
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


void MaximizedWindow::MaximizeMainWindow()
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


void MaximizedWindow::FinishMaximizeMainWindow()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowRounding = OriginalWindowRounding;
}
