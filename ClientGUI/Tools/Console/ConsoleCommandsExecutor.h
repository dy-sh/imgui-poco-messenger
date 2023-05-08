// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <memory>
#include <vector>

#include "imgui.h"
#include "ConsoleCommand.h"
#include "Commands/ClearConsoleCommand.h"
#include "Commands/HelpConsoleCommand.h"
#include "Commands/HistoryConsoleCommand.h"
#include "IConsoleWindow.h"

class ConsoleCommandsExecutor
{
public:
    std::vector<std::unique_ptr<ConsoleCommand>> commands;


    ConsoleCommandsExecutor(IConsoleWindow* window): window{window}
    {
        commands.push_back(std::make_unique<HelpConsoleCommand>("/HELP", this));
        commands.push_back(std::make_unique<HistoryConsoleCommand>("/HISTORY", this));
        commands.push_back(std::make_unique<ClearConsoleCommand>("/CLEAR", this));
    }


    void Print(const char* fmt, ...);
    void ClearHistory();
    std::vector<char*> GetHistory(){return history;}
    void TextCompletion(ImGuiInputTextCallbackData* data);
    void ShowCommandFromHistory(ImGuiInputTextCallbackData* data);
    void ExecCommand(const char* command_line);
    IConsoleWindow* GetConsoleWindow() { return window; }

private:
    IConsoleWindow* window;
    std::vector<char*> history;
    size_t history_pos = -1; // -1: new line, 0..History.Size-1 browsing history.
};
