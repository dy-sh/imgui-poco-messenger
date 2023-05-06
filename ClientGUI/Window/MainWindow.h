//
// Created by Dmitry Savosh on 18.04.2023.
//

#pragma once
#include "Window.h"


// class Client;

class MainWindow : public ResizableWindow
{
public:
    MainWindow(const std::string& title) :ResizableWindow(title){  }
    void RenderContent() override;

private:
    // Client* client;
    bool maximize_main_window = false;
    static float OriginalWindowRounding;
    
//     void MaximizeMainWindow()
//     {
// #ifdef IMGUI_HAS_VIEWPORT
//         ImGuiViewport* viewport = ImGui::GetMainViewport();
//         ImGui::SetNextWindowPos( viewport->WorkPos );
//         ImGui::SetNextWindowSize( viewport->WorkSize );
//         ImGui::SetNextWindowViewport( viewport->ID );
// #else
//         ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
//         ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
// #endif
//
//         ImGuiStyle* style = &ImGui::GetStyle();
//         OriginalWindowRounding = style->WindowRounding;
//         style->WindowRounding = 0.0;
//     }


    // void FinishMaximizeMainWindow()
    // {
    //     ImGuiStyle* style = &ImGui::GetStyle();
    //     style->WindowRounding = OriginalWindowRounding;
    // }
};
