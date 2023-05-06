// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "imgui.h"
#include "Style.h"

struct DefaultStyle : Style
{
    void ApplyStyle(ImVec4& bg_color, ImGuiStyle* dst = nullptr) override
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* mainfont = io.Fonts->AddFontFromFileTTF(
            "C:\\Windows\\Fonts\\Segoeui.ttf", 20.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        IM_ASSERT(mainfont != NULL);

        ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        style->WindowPadding = ImVec2(15, 15);
        style->WindowRounding = 10.0;
        style->FramePadding = ImVec2(5, 5);
        style->ItemSpacing = ImVec2(12, 8);
        style->ItemInnerSpacing = ImVec2(8, 6);
        style->IndentSpacing = 25.0;
        style->ScrollbarSize = 15.0;
        style->ScrollbarRounding = 15.0;
        style->GrabMinSize = 15.0;
        style->GrabRounding = 7.0;
        style->FrameRounding = 6.0;

        constexpr float HUE_G = 1.05;
        constexpr float HUE_B = 1.10;

        constexpr auto Brigter = [](ImVec4 color, float power) -> ImVec4
        {
            return ImVec4(color.x * power, color.y * power, color.z * power, color.w);
        };

        constexpr ImVec4 ACCENT_COLOR_1 = ImVec4(0.28, 0.56, 1.00, 1.00);
        constexpr ImVec4 ACCENT_COLOR_1_BRIGHTER = Brigter(ACCENT_COLOR_1, 1.2);
        constexpr ImVec4 ACCENT_COLOR_2 = ImVec4(0.78, 0.71, 0.44, 1.00);
        constexpr ImVec4 ACCENT_COLOR_2_BRIGHTER = Brigter(ACCENT_COLOR_2, 1.2);
        constexpr ImVec4 ACCENT_COLOR_3 = ImVec4(1.00, 0.43, 0.44, 1.00);
        constexpr ImVec4 ACCENT_COLOR_3_BRIGHTER = Brigter(ACCENT_COLOR_2, 1.2);

        constexpr ImVec4 ELEMENT_DEFAULT_COLOR = ImVec4(0.29, 0.29 * HUE_G, 0.29 * HUE_B, 1.0);
        constexpr ImVec4 ELEMENT_DARK_COLOR = Brigter(ELEMENT_DEFAULT_COLOR, 0.8);
        constexpr ImVec4 ELEMENT_BRIGHT_COLOR = Brigter(ELEMENT_DEFAULT_COLOR, 1.2);
        constexpr ImVec4 ELEMENT_VERY_BRIGHT_COLOR = Brigter(ELEMENT_DEFAULT_COLOR, 1.7);
        constexpr ImVec4 ELEMENT_BG_COLOR_1 = Brigter(ELEMENT_DEFAULT_COLOR, 0.75);
        constexpr ImVec4 ELEMENT_BG_COLOR_2 = Brigter(ELEMENT_DEFAULT_COLOR, 0.85);
        constexpr ImVec4 BACKGROUND_COLOR_1 = ImVec4(0.05, 0.05 * HUE_G, 0.05 * HUE_B, 1.0);
        constexpr ImVec4 BACKGROUND_COLOR_2 = Brigter(BACKGROUND_COLOR_1, 2.0);
        constexpr ImVec4 WINDOW_BG_COLOR = Brigter(BACKGROUND_COLOR_1, 3.0);
        constexpr ImVec4 TITLE_COLOR = Brigter(ELEMENT_DARK_COLOR, 0.5);
        constexpr ImVec4 TITLE_ACTIVE_COLOR = Brigter(ELEMENT_DARK_COLOR, 0.8);
        constexpr ImVec4 TEXT_COLOR = ImVec4(0.95, 0.95 * HUE_G, 0.95 * HUE_B, 1.0);
        constexpr ImVec4 TEXT_DISABLED_COLOR = Brigter(TEXT_COLOR, 0.4f);
        constexpr ImVec4 TRANSPARENT_COLOR = ImVec4(0, 0, 0, 0);

        bg_color = ImVec4(0.11, 0.11 * HUE_G, 0.11 * HUE_B, 1.00);

        colors[ImGuiCol_Text] = TEXT_COLOR;
        colors[ImGuiCol_TextDisabled] = TEXT_DISABLED_COLOR;
        colors[ImGuiCol_TextSelectedBg] = ELEMENT_VERY_BRIGHT_COLOR;
        colors[ImGuiCol_WindowBg] = WINDOW_BG_COLOR;
        colors[ImGuiCol_ChildBg] = WINDOW_BG_COLOR;
        colors[ImGuiCol_MenuBarBg] = ELEMENT_DARK_COLOR;
        colors[ImGuiCol_PopupBg] = ELEMENT_DARK_COLOR;
        colors[ImGuiCol_Border] = BACKGROUND_COLOR_1;
        colors[ImGuiCol_BorderShadow] = BACKGROUND_COLOR_1;
        colors[ImGuiCol_FrameBg] = ELEMENT_BG_COLOR_1;
        colors[ImGuiCol_FrameBgHovered] = ELEMENT_BG_COLOR_2;
        colors[ImGuiCol_FrameBgActive] = ELEMENT_DEFAULT_COLOR;
        colors[ImGuiCol_TitleBg] = TITLE_COLOR;
        colors[ImGuiCol_TitleBgCollapsed] = TITLE_COLOR;
        colors[ImGuiCol_TitleBgActive] = TITLE_ACTIVE_COLOR;
        colors[ImGuiCol_ScrollbarBg] = BACKGROUND_COLOR_2;
        colors[ImGuiCol_ScrollbarGrab] = ELEMENT_DARK_COLOR;
        colors[ImGuiCol_ScrollbarGrabHovered] = ELEMENT_BRIGHT_COLOR;
        colors[ImGuiCol_ScrollbarGrabActive] = ACCENT_COLOR_1;
        colors[ImGuiCol_CheckMark] = ACCENT_COLOR_1_BRIGHTER;
        colors[ImGuiCol_SliderGrab] = ELEMENT_BRIGHT_COLOR;
        colors[ImGuiCol_SliderGrabActive] = ACCENT_COLOR_1_BRIGHTER;
        colors[ImGuiCol_Button] = ELEMENT_DEFAULT_COLOR;
        colors[ImGuiCol_ButtonHovered] = ACCENT_COLOR_1;
        colors[ImGuiCol_ButtonActive] = ACCENT_COLOR_1_BRIGHTER;
        colors[ImGuiCol_Header] = ELEMENT_BRIGHT_COLOR;
        colors[ImGuiCol_HeaderHovered] = ACCENT_COLOR_1;
        colors[ImGuiCol_HeaderActive] = ACCENT_COLOR_1_BRIGHTER;
        colors[ImGuiCol_ResizeGrip] = TRANSPARENT_COLOR;
        colors[ImGuiCol_ResizeGripHovered] = TRANSPARENT_COLOR;
        colors[ImGuiCol_ResizeGripActive] = ACCENT_COLOR_1;
        colors[ImGuiCol_PlotLines] = ACCENT_COLOR_2;
        colors[ImGuiCol_PlotLinesHovered] = ACCENT_COLOR_2_BRIGHTER;
        colors[ImGuiCol_PlotHistogram] = ACCENT_COLOR_2;
        colors[ImGuiCol_PlotHistogramHovered] = ACCENT_COLOR_2_BRIGHTER;
        colors[ImGuiCol_Tab] = ELEMENT_BRIGHT_COLOR;
        colors[ImGuiCol_TabHovered] = ACCENT_COLOR_1;
        colors[ImGuiCol_TabActive] = ACCENT_COLOR_1_BRIGHTER;
        colors[ImGuiCol_TabUnfocused] = ELEMENT_DEFAULT_COLOR;
        colors[ImGuiCol_TabUnfocusedActive] = ELEMENT_BRIGHT_COLOR;
        colors[ImGuiCol_SeparatorActive] = ELEMENT_BRIGHT_COLOR;
        colors[ImGuiCol_SeparatorHovered] = ELEMENT_VERY_BRIGHT_COLOR;
        colors[ImGuiCol_SeparatorActive] = ACCENT_COLOR_1;
        colors[ImGuiCol_DragDropTarget] = ACCENT_COLOR_2_BRIGHTER;
    }
};
