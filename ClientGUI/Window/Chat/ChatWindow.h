//
// Created by Dmitry Savosh on 19.04.2023.
//

#pragma once



#include "imgui.h"
#include "../Window.h"


class CommandsExecutor;
class Client;

struct AppChatColors
{
    ImVec4 ErrorColor   = ImVec4( 1.0f, 0.3f, 0.3f, 1.0f );
    ImVec4 WarningColor = ImVec4( 1.0f, 0.8f, 0.6f, 1.0f );
};

class ChatWindow:public Window
{
public:
    AppChatColors chat_colors;
    CommandsExecutor* commands_executor;

    char InputBuf[256]{};
    ImVector<char*> Items;
    ImGuiTextFilter Filter;
    bool AutoScroll     = true;
    bool ScrollToBottom = false;


    ChatWindow(const std::string& title, bool visible, Client* client);

    ~ChatWindow();
    
    void Clear();
    void Print( const char* fmt, ... );
    void RenderContent() override;
    void Send(const char* s);
    void ProceedMessageTextField();
    int MessageTextEditCallback( ImGuiInputTextCallbackData* data );
private:
    Client* client;
    bool set_focus_on_textfield{true};
};


