//
// Created by Dmitry Savosh on 19.04.2023.
//

#pragma once


#include "imgui.h"
#include "../Window/Window.h"

struct AppChatColors
{
    ImVec4 ErrorColor   = ImVec4( 1.0f, 0.3f, 0.3f, 1.0f );
    ImVec4 WarningColor = ImVec4( 1.0f, 0.8f, 0.6f, 1.0f );
};

class ChatWindow:public Window
{
public:
    AppChatColors ChatColors;

    char InputBuf[256]{};
    static ImVector<char*> Items;
    ImVector<const char*> Commands;
    ImVector<char*> History;
    int HistoryPos = -1; // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll     = true;
    bool ScrollToBottom = false;


    ChatWindow(const std::string& title, bool visible);

    ~ChatWindow();
    
    static void Clear();
    static void Add( const char* fmt, ... ) IM_FMTARGS( 2 );
    void RenderContent() override;
    void Send();
    void ExecCommand( const char* command_line );
    int TextEditCallback( ImGuiInputTextCallbackData* data );
    void Help();
private:
    bool set_focus_on_textfield{true};
};


