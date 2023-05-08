// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once


class ConsoleCommandsExecutor;
struct IConsoleWindow;

struct ConsoleCommand
{
    const char* command;
    ConsoleCommandsExecutor* executor;


    ConsoleCommand(const char* command, ConsoleCommandsExecutor* executor): command{command}, executor{executor}
    {
    }


    virtual void Print(const char* fmt, ...);
    virtual const char* GetDescription() { return command; }
    virtual bool IsValidCommand(const char* command_line);


    virtual void Execute(const char* command_line)
    {
    }
};
