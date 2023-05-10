// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "App.h"

#include "Tools/Debug.h"
#include "imgui.h"
#include "ClientNetworking/Client.h"
#include "Theme/DefaultStyle.h"
#include "Tools/Console.h"
#include "Tools/Log.h"
#include "Window//Chat/ChatWindow.h"
#include "Window/WindowManager.h"
#include "Window/LoginWindow.h"
#include "Window/MainWindow.h"

App::App()
{
    style = std::make_unique<DefaultStyle>();
    client = new Client();
    window_manager.AddWindow(std::make_unique<MainWindow>("Main",  true, &window_manager, client));
    window_manager.AddWindow(std::make_unique<ConsoleWindow>("Console", false));
    window_manager.AddWindow(std::make_unique<LogWindow>("Log", false));
    window_manager.AddWindow(std::make_unique<LoginWindow>("Login", false));
    window_manager.AddWindow(std::make_unique<ChatWindow>("Chat", false, client));
}


App::~App()
{
    window_manager.RemoveAllWindows(); // it important to destroy windows before client
    delete client;
}


void App::ApplyStyle(ImVec4& bg_color, ImGuiStyle* dst)
{
    style->ApplyStyle(bg_color, dst);
}


void App::Render()
{
    if (use_esc_key && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
    {
        Exit();
    }

    if (use_key_for_log && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_X)
        && ImGui::IsKeyDown(ImGuiKey::ImGuiKey_ModAlt))
    {
        if (Window* log_window = window_manager.GetWindowByTitle("Log"))
        {
            log_window->ToggleVisible();
        }
    }

    if (use_key_for_console && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C)
        && ImGui::IsKeyDown(ImGuiKey::ImGuiKey_ModAlt))
    {
        if (Window* console_window = window_manager.GetWindowByTitle("Console"))
        {
            console_window->ToggleVisible();
        }
    }

    if (show_debug_toolbar)
        ShowDebugToolbar();

    window_manager.Render();

    RenderDebugWindows();
}


void App::RenderDebugWindows()
{
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

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


void App::ShowDebugToolbar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Debug"))
        {
            if (Window* console = window_manager.GetWindowByTitle("Console"))
            {
                bool show_console = console->IsVisible();
                ImGui::MenuItem("Console (Alt + C)", NULL, &show_console);
                console->SetVisible(show_console);
            }

            if (Window* console = window_manager.GetWindowByTitle("Log"))
            {
                bool show_log = console->IsVisible();
                ImGui::MenuItem("Log (Alt + X)", NULL, &show_log);
                console->SetVisible(show_log);
            }

            ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
            ImGui::MenuItem("Debug Log", NULL, &show_app_debug_log);
            ImGui::MenuItem("Stack Tool", NULL, &show_app_stack_tool);
            ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
            ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);
            ImGui::MenuItem("Demo Window", NULL, &show_demo_window);
            if (ImGui::MenuItem("Exit (Esc)"))
            {
                Exit();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}


void App::Exit()
{
    should_close = true;
}


void App::OnExit()
{
    window_manager.OnExit();
}
