// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <memory>

#include "Window/WindowManager.h"


struct Style;
class Client;

class AppGUI
{
public:
    Client* client;

    WindowManager window_manager;
    std::unique_ptr<Style> style;

    bool show_debug_toolbar = true;
    bool use_key_for_log = true; // alt + x
    bool use_key_for_console = true; // alt + c
    bool use_esc_key = true; // esc

    bool show_app_log = false;
    bool show_app_metrics = false;
    bool show_app_debug_log = false;
    bool show_app_stack_tool = false;
    bool show_app_about = false;
    bool show_app_style_editor = false;
    bool show_demo_window = false;

    bool should_close = false;


    AppGUI();
    ~AppGUI();

    void ApplyStyle(ImVec4& bg_color, ImGuiStyle* dst = nullptr);
    void Render();
    void RenderDebugWindows();
    void ShowDebugToolbar();
    void Exit();
    void OnExit();
};



