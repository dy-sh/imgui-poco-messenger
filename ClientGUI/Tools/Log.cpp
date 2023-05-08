//
// Created by Dmitry Savosh on 19.04.2023.
//

#include "Log.h"

#include <iomanip>
#include <iostream>
#include <sstream>


LogWindow::LogWindow(const std::string& title, bool visible)
    : Window(title, visible, {700, 400})
{
    Clear();
}


void LogWindow::Clear()
{
    Buf.clear();
    LineOffsets.clear();
    LineOffsets.push_back(0);
}


void LogWindow::AddRaw(const char* fmt, ...)
{
    // write message
    int old_size = Buf.size();
    va_list args;
    va_start(args, fmt);
    Buf.appendfv(fmt, args);
    va_end(args);

    for (int new_size = Buf.size(); old_size < new_size; old_size++)
        if (Buf[old_size] == '\n')
            LineOffsets.push_back(old_size + 1);
}


void LogWindow::Add(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int length = vsnprintf(nullptr, 0, fmt, args); // Determine the length of the formatted string
    std::string buffer(length, '\0'); // Allocate a buffer to store the formatted string
    vsnprintf(&buffer[0], length + 1, fmt, args); // Format the string into the buffer
    Add(LogLevel::inf, "%s", buffer.c_str()); // Add the formatted string to the log
}


void LogWindow::Add(LogLevel level, const char* fmt, ...)
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
    AddRaw("%s", buffer.c_str()); // Add the formatted string to the log

    va_end(args);
}


void LogWindow::RenderContent()
{
    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
    {
        ImGui::OpenPopup("Options");
    }
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    Filter.Draw("Filter", -100.0f);

    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (clear)
        {
            Clear();
        }
        if (copy)
        {
            ImGui::LogToClipboard();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have random access to the result of our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.size(); line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end
                    = (line_no + 1 < LineOffsets.size()) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    DrawColorizedLine(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we have an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(LineOffsets.size());
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end
                        = (line_no + 1 < LineOffsets.size()) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;

                    DrawColorizedLine(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}


void LogWindow::DrawColorizedLine(const char* line_start, const char* line_end) const
{
    static const char* Warning = "wrn";
    static const char* Error = "err";

    if (strncmp(line_start + 1, Warning, strlen(Warning)) == 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, LogColors.WarningColor);
        ImGui::TextUnformatted(line_start, line_end);
        ImGui::PopStyleColor();
    }
    else if (strncmp(line_start + 1, Error, strlen(Error)) == 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, LogColors.ErrorColor);
        ImGui::TextUnformatted(line_start, line_end);
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::TextUnformatted(line_start, line_end);
    }
}


ImGuiTextBuffer LogWindow::Buf{};
std::vector<int> LogWindow::LineOffsets{0};
