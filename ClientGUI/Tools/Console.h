// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once


#include "Debug.h"
#include "imgui.h"
#include "../Window/Window.h"
#include "Console/IConsoleWindow.h"


class ConsoleCommandsExecutor;
class Client;

struct AppConsoleColors
{
    ImVec4 ErrorColor   = ImVec4( 1.0f, 0.3f, 0.3f, 1.0f );
    ImVec4 WarningColor = ImVec4( 1.0f, 0.8f, 0.6f, 1.0f );
};

class ConsoleWindow:public Window, public IConsoleWindow
{
public:
    AppConsoleColors console_colors;
    ConsoleCommandsExecutor* commands_executor;

    char input_buf[256]{};
    static ImVector<char*> items;
    ImGuiTextFilter filter;
    bool auto_scroll     = true;
    bool scroll_to_bottom = false;


    ConsoleWindow(const std::string& title, bool visible);

    ~ConsoleWindow();
    
    void Clear() override;
    void Print( const char* fmt, ... ) override;
    static void Add( const char* fmt, ... );
    static void Add(LogLevel level, const char* fmt, ...);
    void ClearFilter() override{filter.Clear();}
    void RenderContent() override;
    void ProceedMessageTextField();
    int MessageTextEditCallback( ImGuiInputTextCallbackData* data );
private:
    bool set_focus_on_textfield{true};
};


