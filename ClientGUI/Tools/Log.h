//
// Created by Dmitry Savosh on 19.04.2023.
//

#pragma once

#include "imgui.h"
#include "../Window/Window.h"

struct AppLogColors
{
    ImVec4 ErrorColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    ImVec4 WarningColor = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
};

struct AppLogWindow : public Window
{
    AppLogWindow(const std::string& title, bool visible)
        : Window(title, visible, true, {700, 400})
    {
        Clear();
    }


    AppLogColors LogColors;

    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets; // Index to lines offset. We maintain this with AddMessage() calls.
    bool AutoScroll = true; // Keep scrolling if already at the bottom.

    void Clear();
    void Add(const char* fmt, ...) IM_FMTARGS(2);
    void RenderContent() override;
    void DrawColorizedLine(const char* line_start, const char* line_end) const;
};
