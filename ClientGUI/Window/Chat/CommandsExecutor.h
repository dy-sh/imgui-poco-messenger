// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "ChatWindow.h"
#include "imgui.h"

class ChatWindow;

class CommandsExecutor
{
public:
    ImVector<const char*> Commands;
    
    CommandsExecutor(ChatWindow* window):window{window}
    {
        Commands.push_back("/HELP");
        Commands.push_back("/HISTORY");
        Commands.push_back("/CLEAR");
    }
    
    void Print(const char* fmt, ...);
    void ClearHistory();
    void TextCompletion(ImGuiInputTextCallbackData* data);
    void ShowCommandFromHistory(ImGuiInputTextCallbackData* data);
    void ExecCommand( const char* command_line );
    void Help();


private:
    ChatWindow* window;
    ImVector<char*> History;
    int HistoryPos = -1; // -1: new line, 0..History.Size-1 browsing history.
};
