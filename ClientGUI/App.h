//
// Created by Dmitry Savosh on 19.04.2023.
//

#pragma once

#include "Debug\Debug.h"
#include "imgui.h"
#include "Theme\DefaultStyle.h"
#include "Window\WindowManager.h"
#include "window\LoginWindow.h"
#include "window\MainWindow.h"

bool show_debug_toolbar = true;
bool use_key_for_log = true; // alt + x
bool use_key_for_console = true; // alt + c
bool use_esc_key = true; // esc

bool show_login_window = false;
bool show_main_window = true;

bool show_app_console = false;
bool show_app_log = false;
bool show_app_metrics = false;
bool show_app_debug_log = false;
bool show_app_stack_tool = false;
bool show_app_about = false;
bool show_app_style_editor = false;
bool show_demo_window = false;

// extern GLFWwindow* g_window;

static void ShowDebugToolbar();


class App
{
public:
    WindowManager windowManager;
    std::unique_ptr<Style> style;

    App()
    {
        style = std::make_unique<DefaultStyle>();
        windowManager.addWindow(std::make_unique<Window>("Window 1"));
        // windowManager.addWindow(std::make_unique<ModalWindow>("Modal Window"));
        windowManager.addWindow(std::make_unique<ResizableWindow>("Resizable Window"));
        windowManager.addWindow(std::make_unique<MainWindow>("Main Window 1"));
    }



    void ApplyStyle(ImVec4& bg_color, ImGuiStyle* dst = nullptr)
    {
        style->ApplyStyle(bg_color, dst);
    }


    void Render()
    {
        // if( use_esc_key && ImGui::IsKeyPressed( ImGuiKey::ImGuiKey_Escape ) )
        //     glfwSetWindowShouldClose( g_window, true );

        if (use_key_for_log && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_X)
            && ImGui::IsKeyDown(ImGuiKey::ImGuiKey_ModAlt))
            show_app_log = !show_app_log;

        if (use_key_for_console && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C)
            && ImGui::IsKeyDown(ImGuiKey::ImGuiKey_ModAlt))
            show_app_console = !show_app_console;

        if (show_debug_toolbar)
            ShowDebugToolbar();

        if (show_login_window)
            ShowLoginWindow();

        windowManager.render();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        if (show_app_console)
            AppConsole::Draw("Console", &show_app_console);
        if (show_app_log)
            AppLog::Draw("Log", &show_app_log);
        if (show_app_metrics)
            ImGui::ShowMetricsWindow(&show_app_metrics);
        if (show_app_debug_log)
            ImGui::ShowDebugLogWindow(&show_app_debug_log);
        if (show_app_stack_tool)
            ImGui::ShowStackToolWindow(&show_app_stack_tool);
        if (show_app_about)
            ImGui::ShowAboutWindow(&show_app_about);
        if (show_app_style_editor)
        {
            ImGui::Begin("Dear ImGui Style Editor", &show_app_style_editor);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }
    }


    void ShowDebugToolbar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem("Console", NULL, &show_app_console);
                ImGui::MenuItem("Log", NULL, &show_app_log);
                ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
                ImGui::MenuItem("Debug Log", NULL, &show_app_debug_log);
                ImGui::MenuItem("Stack Tool", NULL, &show_app_stack_tool);
                ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
                ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);
                ImGui::MenuItem("Demo Window", NULL, &show_demo_window);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

private:
};
