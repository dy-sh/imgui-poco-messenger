//
// Created by Dmitry Savosh on 19.04.2023.
//

#pragma once

#include "imgui.h"
#include "../Window/Window.h"

struct AppConsoleColors
{
    ImVec4 ErrorColor   = ImVec4( 1.0f, 0.3f, 0.3f, 1.0f );
    ImVec4 WarningColor = ImVec4( 1.0f, 0.8f, 0.6f, 1.0f );
};

class AppConsoleWindow:public Window
{
public:
    AppConsoleColors ConsoleColors;

    char InputBuf[256]{};
    ImVector<char*> Items;
    ImVector<const char*> Commands;
    ImVector<char*> History;
    int HistoryPos = -1; // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll     = true;
    bool ScrollToBottom = false;


    AppConsoleWindow(const std::string& title, bool visible)
        : Window(title, visible, true, {700, 400})
    {
        ClearLog();
        Commands.push_back( "HELP" );
        Commands.push_back( "HISTORY" );
        Commands.push_back( "CLEAR" );
    }
    ~AppConsoleWindow();
    
    void ClearLog();
    void Add( const char* fmt, ... ) IM_FMTARGS( 2 );
    void RenderContent() override;
    void ExecCommand( const char* command_line );
    int TextEditCallback( ImGuiInputTextCallbackData* data );
    void Help();
};


