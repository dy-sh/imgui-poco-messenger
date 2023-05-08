#include "AppGUI.h"

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

AppGUI::AppGUI()
{
    client = new Client();
    style = std::make_unique<DefaultStyle>();
    windowManager.AddWindow(std::make_unique<MainWindow>("Main",  true, &windowManager, client));
    windowManager.AddWindow(std::make_unique<ConsoleWindow>("Console", false));
    windowManager.AddWindow(std::make_unique<LogWindow>("Log", false));
    windowManager.AddWindow(std::make_unique<LoginWindow>("Login", false));
    windowManager.AddWindow(std::make_unique<ChatWindow>("Chat", true, client));
}


AppGUI::~AppGUI()
{
    delete client;
}


void AppGUI::ApplyStyle(ImVec4& bg_color, ImGuiStyle* dst)
{
    style->ApplyStyle(bg_color, dst);
}


void AppGUI::Render()
{
    if (use_esc_key && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
    {
        Exit();
    }

    if (use_key_for_log && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_X)
        && ImGui::IsKeyDown(ImGuiKey::ImGuiKey_ModAlt))
    {
        if (Window* logWindow = windowManager.GetWindowByTitle("Log"))
        {
            logWindow->ToggleVisible();
        }
    }

    if (use_key_for_console && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_C)
        && ImGui::IsKeyDown(ImGuiKey::ImGuiKey_ModAlt))
    {
        if (Window* consoleWindow = windowManager.GetWindowByTitle("Console"))
        {
            consoleWindow->ToggleVisible();
        }
    }

    if (show_debug_toolbar)
        ShowDebugToolbar();

    windowManager.Render();

    RenderDebugWindows();
}


void AppGUI::RenderDebugWindows()
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


void AppGUI::ShowDebugToolbar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Debug"))
        {
            if (Window* console = windowManager.GetWindowByTitle("Console"))
            {
                bool show_console = console->IsVisible();
                ImGui::MenuItem("Console (Alt + C)", NULL, &show_console);
                console->SetVisible(show_console);
            }

            if (Window* console = windowManager.GetWindowByTitle("Log"))
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


void AppGUI::Exit()
{
    should_close = true;
}


void AppGUI::OnExit()
{
    windowManager.OnExit();
}
