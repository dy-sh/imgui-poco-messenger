// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#define _CRT_SECURE_NO_WARNINGS 1

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include "../Utils/Utils.h"
#include "Console.h"
#include "Console/ConsoleCommandsExecutor.h"


static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    auto* Console = (ConsoleWindow*)data->UserData;
    return Console->MessageTextEditCallback(data);
}


ConsoleWindow::ConsoleWindow(const std::string& title, bool visible)
    : Window(title, visible, {700, 400})
{
    commands_executor = new ConsoleCommandsExecutor(this);
    Clear();
}


ConsoleWindow::~ConsoleWindow()
{
    Clear();
    delete commands_executor;
}


void ConsoleWindow::Clear()
{
    for (int i = 0; i < items.Size; i++)
        free(items[i]);
    items.clear();
}


void ConsoleWindow::Print(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int length = vsnprintf(nullptr, 0, fmt, args); // Determine the length of the formatted string
    std::string buffer(length, '\0'); // Allocate a buffer to store the formatted string
    vsnprintf(&buffer[0], length + 1, fmt, args); // Format the string into the buffer
    Add("%s", buffer.c_str()); // Add the formatted string to the log

    va_end(args);
}


void ConsoleWindow::Add(const char* fmt, ...)
{
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    items.push_back(Strdup(buf));
}

void ConsoleWindow::Add(LogLevel level, const char* fmt, ...)
{
    std::ostringstream oss;

    // append LogLevel
    oss << "[" << ToString(level) << "] ";
    // append current time
    oss << "[" << std::fixed << std::setprecision(2) << ImGui::GetTime() << "] ";
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


void ConsoleWindow::RenderContent()
{
    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
    // So e.g. IsItemHovered() will return true when hovering the title bar.
    // Here we create a context menu only available from the title bar.
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close"))
        {
            visible = false;
        }
        ImGui::EndPopup();
    }

    // TODO: display items starting from the bottom

    if (ImGui::SmallButton("Clear"))
    {
        Clear();
    }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    ImGui::SameLine();
    if (ImGui::SmallButton("Help"))
    {
        commands_executor->ExecCommand("/HELP");
    }

    static bool spam = false;
    ImGui::SameLine();
    if (ImGui::SmallButton("Spam"))
        spam = !spam;
    if (spam)
        Print("Spam %f", ImGui::GetTime());

    //    ImGui::Separator();

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &auto_scroll);
        ImGui::EndPopup();
    }

    // Options, Filter
    ImGui::SameLine();
    if (ImGui::SmallButton("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    filter.Draw("Filter", 180);
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild(
        "ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear"))
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
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();
        for (int i = 0; i < items.Size; i++)
        {
            const char* item = items[i];
            if (!filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[err]"))
            {
                color = console_colors.ErrorColor;
                has_color = true;
            }
            else if (strstr(item, "[wrn]"))
            {
                color = console_colors.WarningColor;
                has_color = true;
            }
            else if (strncmp(item, "# ", 2) == 0) // user command
            {
                color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
                has_color = true;
            }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (scroll_to_bottom || (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        scroll_to_bottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::Separator();


    // Auto-focus on text field
    ImGui::SetItemDefaultFocus();
    if (set_focus_on_textfield)
    {
        ImGui::SetKeyboardFocusHere(); // Auto focus next widget
        set_focus_on_textfield = false;
    }

    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll
        | ImGuiInputTextFlags_CallbackCompletion
        | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Command", input_buf, IM_ARRAYSIZE(input_buf), input_text_flags, &TextEditCallbackStub,
                         (void*)this))
    {
        ProceedMessageTextField();
        set_focus_on_textfield = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Execute"))
    {
        ProceedMessageTextField();
        set_focus_on_textfield = true;
    }
}




void ConsoleWindow::ProceedMessageTextField()
{
    char* s = input_buf;
    Strtrim(s);

    if (s[0] != 0) //not empty line
    {
        commands_executor->ExecCommand(s);
        scroll_to_bottom = true;
    }

    strcpy(s, "");
}


int ConsoleWindow::MessageTextEditCallback(ImGuiInputTextCallbackData* data)
{
    // AddMessage("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
        {
            commands_executor->TextCompletion(data);
            break;
        }
    case ImGuiInputTextFlags_CallbackHistory:
        {
            commands_executor->ShowCommandFromHistory(data);
        }
    }
    return 0;
}


ImVector<char*> ConsoleWindow::items {};