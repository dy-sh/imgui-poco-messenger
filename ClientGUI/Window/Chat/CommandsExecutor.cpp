#include "CommandsExecutor.h"

#include "ChatWindow.h"
#include "../../Utils/Utils.h"
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

void CommandsExecutor::Print(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int length = vsnprintf(nullptr, 0, fmt, args); // Determine the length of the formatted string
    std::string buffer(length, '\0'); // Allocate a buffer to store the formatted string
    vsnprintf(&buffer[0], length + 1, fmt, args); // Format the string into the buffer
    window->Print( "%s", buffer.c_str()); // Add the formatted string to the log
}



void CommandsExecutor::ClearHistory()
{
    for (int i = 0; i < History.Size; i++)
        free(History[i]);
}

void CommandsExecutor::TextCompletion(ImGuiInputTextCallbackData* data)
{
    // Example of TEXT COMPLETION

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
    for (int i = 0; i < Commands.Size; i++)
        if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
            candidates.push_back(Commands[i]);

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

void CommandsExecutor::ShowCommandFromHistory(ImGuiInputTextCallbackData* data)
{
    // Example of HISTORY
    const int prev_history_pos = HistoryPos;
    if (data->EventKey == ImGuiKey_UpArrow)
    {
        if (HistoryPos == -1)
            HistoryPos = History.Size - 1;
        else if (HistoryPos > 0)
            HistoryPos--;
    }
    else if (data->EventKey == ImGuiKey_DownArrow)
    {
        if (HistoryPos != -1)
            if (++HistoryPos >= History.Size)
                HistoryPos = -1;
    }

    // A better implementation would preserve the data on the current input line along with cursor position.
    if (prev_history_pos != HistoryPos)
    {
        const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, history_str);
    }
}

void CommandsExecutor::ExecCommand(const char* command_line)
{
    Print("# %s\n", command_line);

    // Insert into history. First find match and delete it so it can be pushed to the back.
    // This isn't trying to be smart or optimal.
    HistoryPos = -1;
    for (int i = History.Size - 1; i >= 0; i--)
        if (Stricmp(History[i], command_line) == 0)
        {
            free(History[i]);
            History.erase(History.begin() + i);
            break;
        }
    History.push_back(Strdup(command_line));

    // Process command
    if (Stricmp(command_line, "/CLEAR") == 0)
    {
        window->Clear();
    }
    else if (Stricmp(command_line, "/HELP") == 0)
    {
        Help();
    }
    else if (Stricmp(command_line, "/HISTORY") == 0)
    {
        int first = History.Size - 10;
        for (int i = first > 0 ? first : 0; i < History.Size; i++)
            Print("%3d: %s\n", i, History[i]);
    }
    else
    {
        Print("Unknown command: '%s'\n", command_line);
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    window->ScrollToBottom = true;
}


void CommandsExecutor::Help()
{
    Print("This example implements a Chat with basic coloring, completion (TAB key) and history (Up/Down keys). A "
        "more elaborate "
        "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    Print("Log levels:");
    Print("[inf] something ok");
    Print("[wrn] something important");
    Print("[err] something critical");
    Print(R"(Filter syntax:  "inclide, -exclude" (example: "help, -hist, -wrn"))");
    Print("Commands:");
    for (int i = 0; i < Commands.Size; i++)
        Print("- %s", Commands[i]);

    window->Filter.Clear();
}



