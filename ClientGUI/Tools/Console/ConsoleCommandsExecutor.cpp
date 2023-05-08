// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>


#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include "ConsoleCommandsExecutor.h"
#include "../../Utils/Utils.h"


void ConsoleCommandsExecutor::Print(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int length = vsnprintf(nullptr, 0, fmt, args); // Determine the length of the formatted string
    std::string buffer(length, '\0'); // Allocate a buffer to store the formatted string
    vsnprintf(&buffer[0], length + 1, fmt, args); // Format the string into the buffer
    window->Print("%s", buffer.c_str()); // Add the formatted string to the log
}


void ConsoleCommandsExecutor::ClearHistory()
{
    for (int i = 0; i < history.size(); i++)
        free(history[i]);
}


void ConsoleCommandsExecutor::TextCompletion(ImGuiInputTextCallbackData* data)
{
    // Locate beginning of current word
    const char* word_end = data->Buf + data->CursorPos;
    const char* word_start = word_end;
    while (word_start > data->Buf)
    {
        const char c = word_start[-1];
        if (c == ' ' || c == '\t' || c == ',' || c == ';')
            break;
        word_start--;
    }

    // Build a list of candidates
    ImVector<const char*> candidates;
    for (int i = 0; i < commands.size(); i++)
        if (Strnicmp(commands[i]->command, word_start, (int)(word_end - word_start)) == 0)
            candidates.push_back(commands[i]->command);

    if (candidates.Size == 0)
    {
        // No match
        Print("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
    }
    else if (candidates.Size == 1)
    {
        // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
        data->InsertChars(data->CursorPos, candidates[0]);
        data->InsertChars(data->CursorPos, " ");
    }
    else
    {
        // Multiple matches. Complete as much as we can..
        // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
        int match_len = (int)(word_end - word_start);
        for (;;)
        {
            int c = 0;
            bool all_candidates_matches = true;
            for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                if (i == 0)
                    c = toupper(candidates[i][match_len]);
                else if (c == 0 || c != toupper(candidates[i][match_len]))
                    all_candidates_matches = false;
            if (!all_candidates_matches)
                break;
            match_len++;
        }

        if (match_len > 0)
        {
            data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
        }

        // List matches
        Print("Possible matches:\n");
        for (int i = 0; i < candidates.Size; i++)
            Print("- %s\n", candidates[i]);
    }
}


void ConsoleCommandsExecutor::ShowCommandFromHistory(ImGuiInputTextCallbackData* data)
{
    // Example of HISTORY
    const int prev_history_pos = history_pos;
    if (data->EventKey == ImGuiKey_UpArrow)
    {
        if (history_pos == -1)
            history_pos = history.size() - 1;
        else if (history_pos > 0)
            history_pos--;
    }
    else if (data->EventKey == ImGuiKey_DownArrow)
    {
        if (history_pos != -1)
            if (++history_pos >= history.size())
                history_pos = -1;
    }

    // A better implementation would preserve the data on the current input line along with cursor position.
    if (prev_history_pos != history_pos)
    {
        const char* history_str = (history_pos >= 0) ? history[history_pos] : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, history_str);
    }
}


void ConsoleCommandsExecutor::ExecCommand(const char* command_line)
{
    Print("# %s\n", command_line);

    // Insert into history. First find match and delete it so it can be pushed to the back.
    // This isn't trying to be smart or optimal.
    history_pos = -1;
    for (int i = history.size() - 1; i >= 0; i--)
        if (Stricmp(history[i], command_line) == 0)
        {
            free(history[i]);
            history.erase(history.begin() + i);
            break;
        }
    history.push_back(Strdup(command_line));

    bool found = false;
    for (auto& command : commands)
    {
        if (command->IsValidCommand(command_line))
        {
            command->Execute(command_line);
            found = true;
            break;
        }
    }
    if (!found)
    {
        Print("Unknown command: '%s'\nUse /HELP for assist\n", command_line);
    }
}
