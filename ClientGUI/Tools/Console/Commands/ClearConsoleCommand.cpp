// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClearConsoleCommand.h"
#include "../ConsoleCommandsExecutor.h"


void ClearConsoleCommand::Execute(const char* command_line)
{
    executor->GetConsoleWindow()->Clear();
}
