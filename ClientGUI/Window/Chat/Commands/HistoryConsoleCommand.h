// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "../ConsoleCommand.h"

struct HistoryConsoleCommand:ConsoleCommand
{
    explicit HistoryConsoleCommand(const char* command, ConsoleCommandsExecutor* executor)
        : ConsoleCommand(command, executor)
    {
    }


    void Execute(const char* command_line) override;
    
};
