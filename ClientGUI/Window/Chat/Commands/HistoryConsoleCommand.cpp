// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "HistoryConsoleCommand.h"
#include "../ConsoleCommandsExecutor.h"


void HistoryConsoleCommand::Execute(const char* command_line)
{
    const std::vector<char*> history = executor->GetHistory();

    int first = history.size() - 10;
    for (int i = first > 0 ? first : 0; i < history.size(); i++)
        Print("%3d: %s\n", i, history[i]);
}
