//
// Created by Dmitry Savosh on 19.04.2023.
//

#pragma once

#include <vector>

#include "Debug.h"
#include "imgui.h"
#include "../Window/Window.h"

struct AppLogColors
{
    ImVec4 ErrorColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    ImVec4 WarningColor = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
};

struct LogWindow : public Window
{
    LogWindow(const std::string& title, bool visible);


    AppLogColors log_colors;

    static ImGuiTextBuffer buffer;
    ImGuiTextFilter filter;
    static std::vector<int> line_offsets;
    bool auto_scroll = true; // Keep scrolling if already at the bottom.

    static void Clear();
    static void AddRaw(const char* fmt, ...);
    static void Add(const char* fmt, ...);
    static void Add(LogLevel level, const char* fmt, ...);
    void RenderContent() override;
    void DrawColorizedLine(const char* line_start, const char* line_end) const;
};
