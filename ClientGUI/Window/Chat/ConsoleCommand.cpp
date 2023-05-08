// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ConsoleCommand.h"
#include <cstdarg>
#include <cstdio>
#include <string>
#include "ConsoleCommandsExecutor.h"
#include "../../Utils/Utils.h"

void ConsoleCommand::Print(const char* fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        int length = vsnprintf(nullptr, 0, fmt, args); // Determine the length of the formatted string
        std::string buffer(length, '\0'); // Allocate a buffer to store the formatted string
        vsnprintf(&buffer[0], length + 1, fmt, args); // Format the string into the buffer
        executor->Print( "%s", buffer.c_str()); // Add the formatted string to the log
    }


bool ConsoleCommand::IsValidCommand(const char* command_line)
{
    return Stricmp(command_line, command) == 0;
}



