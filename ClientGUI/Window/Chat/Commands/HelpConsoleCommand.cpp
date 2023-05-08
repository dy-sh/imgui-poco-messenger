// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "HelpConsoleCommand.h"
#include "../ConsoleCommandsExecutor.h"


void HelpConsoleCommand::Execute(const char* command_line)
{
    Print("Use TAB for text completion, Up/Down keys for history");
    Print("Log levels:");
    Print("[inf] something ok");
    Print("[wrn] something important");
    Print("[err] something critical");
    Print(R"(Filter syntax:  "inclide, -exclude" (example: "help, -hist, -wrn"))");
    Print("Commands:");
    for (int i = 0; i < executor->commands.size(); i++)
        Print("- %s", executor->commands[i]->GetDescription());

    executor->GetConsoleWindow()->ClearFilter();
}
