//
// Created by Dmitry Savosh on 19.04.2023.
//

#define _CRT_SECURE_NO_WARNINGS 1

#include "Console.h"
#include "../Utils/Utils.h"
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

static int TextEditCallbackStub( ImGuiInputTextCallbackData* data )
{
    auto* console = (ConsoleWindow*)data->UserData;
    return console->TextEditCallback( data );
}


ConsoleWindow::ConsoleWindow(const std::string& title, bool visible)
: Window(title, visible, {700, 400})
{
    Clear();
    Commands.push_back( "HELP" );
    Commands.push_back( "HISTORY" );
    Commands.push_back( "CLEAR" );
}


ConsoleWindow::~ConsoleWindow()
{
    Clear();
    for( int i = 0; i < History.Size; i++ )
        free( History[i] );
}

void ConsoleWindow::Clear()
{
    for( int i = 0; i < Items.Size; i++ )
        free( Items[i] );
    Items.clear();
}

void ConsoleWindow::Add( const char* fmt, ... )
{
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start( args, fmt );
    vsnprintf( buf, IM_ARRAYSIZE( buf ), fmt, args );
    buf[IM_ARRAYSIZE( buf ) - 1] = 0;
    va_end( args );
    Items.push_back( Strdup( buf ) );
}



void ConsoleWindow::Add(LogLevel level, const char* fmt, ...)
{
    std::ostringstream oss;

    // append LogLevel
    oss << "[" << ToString(level) << "] ";
    // append message
    oss << fmt << "\n";

    std::string s = oss.str();
    fmt = s.c_str();

    va_list args;
    va_start(args, fmt);

    int length = vsnprintf(nullptr, 0, fmt, args); // Determine the length of the formatted string
    std::string buffer(length, '\0'); // Allocate a buffer to store the formatted string
    vsnprintf(&buffer[0], length + 1, fmt, args); // Format the string into the buffer
    Add("%s", buffer.c_str()); // Add the formatted string to the log

    va_end(args);
}

void ConsoleWindow::RenderContent( )
{
    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
    // So e.g. IsItemHovered() will return true when hovering the title bar.
    // Here we create a context menu only available from the title bar.
    if( ImGui::BeginPopupContextItem() )
    {
        if( ImGui::MenuItem( "Close" ) )
        {
            visible = false;
        }
        ImGui::EndPopup();
    }

    // TODO: display items starting from the bottom

    if( ImGui::SmallButton( "Clear" ) )
    {
        Clear();
    }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton( "Copy" );
    ImGui::SameLine();
    if( ImGui::SmallButton( "Help" ) )
    {
        Help();
    }

    static bool spam = false;
    ImGui::SameLine();
    if( ImGui::SmallButton( "Spam" ) )
        spam = !spam;
    if( spam )
        Add( "Spam %f", ImGui::GetTime() );

    //    ImGui::Separator();

    // Options menu
    if( ImGui::BeginPopup( "Options" ) )
    {
        ImGui::Checkbox( "Auto-scroll", &AutoScroll );
        ImGui::EndPopup();
    }

    // Options, Filter
    ImGui::SameLine();
    if( ImGui::SmallButton( "Options" ) )
        ImGui::OpenPopup( "Options" );
    ImGui::SameLine();
    Filter.Draw( "Filter", 180 );
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if( ImGui::BeginChild(
            "ScrollingRegion", ImVec2( 0, -footer_height_to_reserve ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
    {
        if( ImGui::BeginPopupContextWindow() )
        {
            if( ImGui::Selectable( "Clear" ) )
                Clear();
            ImGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 1 ) ); // Tighten spacing
        if( copy_to_clipboard )
            ImGui::LogToClipboard();
        for( int i = 0; i < Items.Size; i++ )
        {
            const char* item = Items[i];
            if( !Filter.PassFilter( item ) )
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if( strstr( item, "[err]" ) )
            {
                color     = ConsoleColors.ErrorColor;
                has_color = true;
            }
            else if( strstr( item, "[wrn]" ) )
            {
                color     = ConsoleColors.WarningColor;
                has_color = true;
            }
            else if( strncmp( item, "# ", 2 ) == 0 ) // user command
            {
                color     = ImVec4( 1.0f, 0.8f, 0.6f, 1.0f );
                has_color = true;
            }
            if( has_color )
                ImGui::PushStyleColor( ImGuiCol_Text, color );
            ImGui::TextUnformatted( item );
            if( has_color )
                ImGui::PopStyleColor();
        }
        if( copy_to_clipboard )
            ImGui::LogFinish();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if( ScrollToBottom || ( AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() ) )
            ImGui::SetScrollHereY( 1.0f );
        ScrollToBottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus                   = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll
                                           | ImGuiInputTextFlags_CallbackCompletion
                                           | ImGuiInputTextFlags_CallbackHistory;
    if( ImGui::InputText(
            "Input", InputBuf, IM_ARRAYSIZE( InputBuf ), input_text_flags, &TextEditCallbackStub, (void*)this ) )
    {
        char* s = InputBuf;
        Strtrim( s );
        if( s[0] )
            ExecCommand( s );
        strcpy( s, "" );
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if( reclaim_focus )
        ImGui::SetKeyboardFocusHere( -1 ); // Auto focus previous widget
}

void ConsoleWindow::ExecCommand( const char* command_line )
{
    Add( "# %s\n", command_line );

    // Insert into history. First find match and delete it so it can be pushed to the back.
    // This isn't trying to be smart or optimal.
    HistoryPos = -1;
    for( int i = History.Size - 1; i >= 0; i-- )
        if( Stricmp( History[i], command_line ) == 0 )
        {
            free( History[i] );
            History.erase( History.begin() + i );
            break;
        }
    History.push_back( Strdup( command_line ) );

    // Process command
    if( Stricmp( command_line, "CLEAR" ) == 0 )
    {
        Clear();
    }
    else if( Stricmp( command_line, "HELP" ) == 0 )
    {
        Help();
    }
    else if( Stricmp( command_line, "HISTORY" ) == 0 )
    {
        int first = History.Size - 10;
        for( int i = first > 0 ? first : 0; i < History.Size; i++ )
            Add( "%3d: %s\n", i, History[i] );
    }
    else
    {
        Add( "Unknown command: '%s'\n", command_line );
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    ScrollToBottom = true;
}
void ConsoleWindow::Help()
{
    Add( "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A "
         "more elaborate "
         "implementation may want to store entries along with extra data such as timestamp, emitter, etc." );
    Add( "Log levels:" );
    Add( "[inf] something ok" );
    Add( "[wrn] something important" );
    Add( "[err] something critical" );
    Add( R"(Filter syntax:  "inclide, -exclude" (example: "help, -hist, -wrn"))" );
    Add( "Commands:" );
    for( int i = 0; i < Commands.Size; i++ )
        Add( "- %s", Commands[i] );

    Filter.Clear();
}

int ConsoleWindow::TextEditCallback( ImGuiInputTextCallbackData* data )
{
    // AddMessage("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch( data->EventFlag )
    {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end   = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while( word_start > data->Buf )
            {
                const char c = word_start[-1];
                if( c == ' ' || c == '\t' || c == ',' || c == ';' )
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for( int i = 0; i < Commands.Size; i++ )
                if( Strnicmp( Commands[i], word_start, (int)( word_end - word_start ) ) == 0 )
                    candidates.push_back( Commands[i] );

            if( candidates.Size == 0 )
            {
                // No match
                Add( "No match for \"%.*s\"!\n", (int)( word_end - word_start ), word_start );
            }
            else if( candidates.Size == 1 )
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                data->DeleteChars( (int)( word_start - data->Buf ), (int)( word_end - word_start ) );
                data->InsertChars( data->CursorPos, candidates[0] );
                data->InsertChars( data->CursorPos, " " );
            }
            else
            {
                // Multiple matches. Complete as much as we can..
                // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                int match_len = (int)( word_end - word_start );
                for( ;; )
                {
                    int c                       = 0;
                    bool all_candidates_matches = true;
                    for( int i = 0; i < candidates.Size && all_candidates_matches; i++ )
                        if( i == 0 )
                            c = toupper( candidates[i][match_len] );
                        else if( c == 0 || c != toupper( candidates[i][match_len] ) )
                            all_candidates_matches = false;
                    if( !all_candidates_matches )
                        break;
                    match_len++;
                }

                if( match_len > 0 )
                {
                    data->DeleteChars( (int)( word_start - data->Buf ), (int)( word_end - word_start ) );
                    data->InsertChars( data->CursorPos, candidates[0], candidates[0] + match_len );
                }

                // List matches
                Add( "Possible matches:\n" );
                for( int i = 0; i < candidates.Size; i++ )
                    Add( "- %s\n", candidates[i] );
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if( data->EventKey == ImGuiKey_UpArrow )
            {
                if( HistoryPos == -1 )
                    HistoryPos = History.Size - 1;
                else if( HistoryPos > 0 )
                    HistoryPos--;
            }
            else if( data->EventKey == ImGuiKey_DownArrow )
            {
                if( HistoryPos != -1 )
                    if( ++HistoryPos >= History.Size )
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if( prev_history_pos != HistoryPos )
            {
                const char* history_str = ( HistoryPos >= 0 ) ? History[HistoryPos] : "";
                data->DeleteChars( 0, data->BufTextLen );
                data->InsertChars( 0, history_str );
            }
        }
    }
    return 0;
}

ImVector<char*> ConsoleWindow::Items {};