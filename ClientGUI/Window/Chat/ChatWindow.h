// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once


#include <Poco/AbstractDelegate.h>

#include "imgui.h"
#include "../Window.h"
#include "../../Tools/Console/IConsoleWindow.h"


struct Message;
class ConsoleCommandsExecutor;
class Client;

struct AppChatColors
{
    ImVec4 ErrorColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    ImVec4 WarningColor = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
};

class ChatWindow : public MaximizedWindow, public IConsoleWindow
{
public:
    AppChatColors chat_colors;
    ConsoleCommandsExecutor* commands_executor;

    char input_buf[256]{};
    ImVector<char*> items;
    ImGuiTextFilter filter;
    bool auto_scroll = true;
    bool scroll_to_bottom = false;
    int room_selected = 1;
    bool spam = false;
    bool search = false;


    ChatWindow(const std::string& title, bool visible, Client* client);

    ~ChatWindow();

    void Clear() override;
    void Print(const char* fmt, ...) override;
    void Print(std::string fmt, ...);
    void ClearFilter() override { filter.Clear(); }
    void RenderContent() override;
    void Send(const char* message);
    void ProceedMessageTextField();
    int MessageTextEditCallback(ImGuiInputTextCallbackData* data);
    void OnReceiveMessage(const void* sender, Message*& message);

private:
    Client* client;
    bool set_focus_on_textfield{true};
};
