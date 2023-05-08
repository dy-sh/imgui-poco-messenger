// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include "../ConsoleCommand.h"

struct HelpConsoleCommand : ConsoleCommand
{
    explicit HelpConsoleCommand(const char* command, ConsoleCommandsExecutor* executor)
        : ConsoleCommand(command, executor)
    {
    }


    void Execute(const char* command_line) override;
};
