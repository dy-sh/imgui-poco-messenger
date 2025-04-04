﻿// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#define _CRT_SECURE_NO_WARNINGS 1

#include "ChatWindow.h"

#include <string>
#include <vector>
#include <Poco/Format.h>
#include "../../Utils/Utils.h"
#include "../../Tools/Console/ConsoleCommandsExecutor.h"
#include "ClientNetworking/Client.h"
#include "Protocol/Message.h"
#include "Protocol/Messages/ClientCreateRoomMessage.h"
#include "Protocol/Messages/ClientGetRoomsMessage.h"
#include "Protocol/Messages/ClientTextMessage.h"
#include "Protocol/Messages/ServerAuthorizeMessage.h"
#include "Protocol/Messages/ServerJoinMessage.h"
#include "Protocol/Messages/ServerLeaveMessage.h"
#include "Protocol/Messages/ServerRoomListMessage.h"
#include "Protocol/Messages/ServerTextMessage.h"
using Poco::delegate;
using std::string;
using std::to_string;


static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    auto* Chat = (ChatWindow*)data->UserData;
    return Chat->MessageTextEditCallback(data);
}


ChatWindow::ChatWindow(const std::string& title, bool visible, Client* client)
    : MaximizedWindow(title, visible), client{client}
{
    commands_executor = new ConsoleCommandsExecutor(this);
    client->OnReceiveMessage += delegate(this, &ChatWindow::OnReceiveMessage);
    Clear();
}


ChatWindow::~ChatWindow()
{
    delete commands_executor;
    client->OnReceiveMessage -= delegate(this, &ChatWindow::OnReceiveMessage);
    Clear();
}


void ChatWindow::Clear()
{
    for (auto& [id,room] : rooms)
    {
        for (int i = 0; i < room->items.size(); i++)
        {
            free(room->items[i]);
        }
        room->items.clear();
    }
}


void ChatWindow::Print(const char* fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);

    Room* room = GetActiveRoom();
    room->items.push_back(Strdup(buf));
}


void ChatWindow::Print(std::string fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt.c_str(), args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);

    Room* room = GetActiveRoom();
    room->items.push_back(Strdup(buf));
}


void ChatWindow::Print(int room_id, std::string fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt.c_str(), args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);

    Room* room = GetRoom(room_id, true);
    room->items.push_back(Strdup(buf));
}


void ChatWindow::RenderContent()
{
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


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


    // Room list
    ImGui::BeginGroup();

    ImGui::BeginChild("left top pane", ImVec2(200, -ImGui::GetFrameHeightWithSpacing()), true);
    for (auto& [room_id,room] : rooms)
    {
        char label[128];
        std::string new_mess = GetRoom(room_id)->new_messages ? "(*)" : "";
        sprintf(label, "%s %s", room->name.c_str(), new_mess.c_str());
        if (ImGui::Selectable(label, selected_room_id == room_id))
        {
            selected_room_id = room_id;
            GetRoom(selected_room_id)->new_messages = false;
        }
    }
    ImGui::EndChild();
    if (ImGui::Button("<", ImVec2(90.0f, 0)))
    {
        if (client)
        {
            client->Disconnect();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("+", ImVec2(90.0f, 0)))
    {
        if (client)
        {
            std::string mess = ClientCreateRoomMessage::Serialize("Room");
            client->Send(mess);
        }
    }
    ImGui::EndGroup();
    ImGui::SameLine();

    // Chat
    {
        ImGui::BeginGroup();
        ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        // Leave room for 1 line below us
        ImGui::Text("Room: %d", selected_room_id);
        ImGui::Separator();


        {
            if (search)
            {
                filter.Draw("Search", 180);
                ImGui::Separator();
            }

            // Reserve enough left-over height for 1 separator + 1 input text
            const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing()
                - 30;
            bool copy_to_clipboard = false;

            if (ImGui::BeginChild(
                "ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
            {
                if (ImGui::BeginPopupContextWindow())
                {
                    ImGui::Checkbox("Auto-scroll", &auto_scroll);
                    ImGui::Checkbox("Search", &search);


                    if (ImGui::Selectable("Clear"))
                    {
                        Clear();
                    }
                    if (ImGui::Selectable("Copy"))
                    {
                        copy_to_clipboard = true;
                    }
                    if (ImGui::Selectable("Help"))
                    {
                        commands_executor->ExecCommand("/HELP");
                    }
                    if (ImGui::Selectable("Spam"))
                    {
                        spam = !spam;
                    }
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
                {
                    ImGui::LogToClipboard();
                }

                Room* room = GetActiveRoom();

                for (int i = 0; i < room->items.size(); i++)
                {
                    const char* item = room->items[i];
                    if (!filter.PassFilter(item))
                        continue;

                    // Normally you would store more information in your item than just a string.
                    // (e.g. make Items[] an array of structure, store color/type etc.)
                    ImVec4 color;
                    bool has_color = false;
                    if (strstr(item, "[err]"))
                    {
                        color = chat_colors.ErrorColor;
                        has_color = true;
                    }
                    else if (strstr(item, "[wrn]"))
                    {
                        color = chat_colors.WarningColor;
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
        }
        if (spam)
        {
            std::string s = Poco::format("Spam %f", ImGui::GetTime());
            Send(s);
        }

        ImGui::EndChild();

        // Auto-focus on text field
        ImGui::SetItemDefaultFocus();
        if (set_focus_on_textfield)
        {
            ImGui::SetKeyboardFocusHere(); // Auto focus next widget
            set_focus_on_textfield = false;
        }

        ImGui::SetNextItemWidth(ImGui::GetFrameHeightWithSpacing() - 270);
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue |
            ImGuiInputTextFlags_EscapeClearsAll
            | ImGuiInputTextFlags_CallbackCompletion
            | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText(" ", input_buf, IM_ARRAYSIZE(input_buf), input_text_flags, &TextEditCallbackStub,
                             (void*)this))
        {
            ProceedMessageTextField();
            set_focus_on_textfield = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("Send", ImVec2(200.0f, 0)))
        {
            ProceedMessageTextField();
            set_focus_on_textfield = true;
        }


        ImGui::EndGroup();
    }
}


void ChatWindow::Send(std::string message)
{
    std::string mess = ClientTextMessage::Serialize(selected_room_id, message);
    client->Send(mess);
}


void ChatWindow::Send(const char* message)
{
    Send(std::string(message));
}


void ChatWindow::ProceedMessageTextField()
{
    char* s = input_buf;
    Strtrim(s);

    if (s[0] == '/') //command
    {
        commands_executor->ExecCommand(s);
        scroll_to_bottom = true;
    }

    if (s[0] != 0) //not empty line
    {
        Send(s);
    }

    strcpy(s, "");
}


int ChatWindow::MessageTextEditCallback(ImGuiInputTextCallbackData* data)
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


void ChatWindow::OnReceiveMessage(const void* sender, Message*& message)
{
    if (auto mess = dynamic_cast<ServerTextMessage*>(message))
    {
        Print(mess->room_id, mess->user_name + " : " + mess->text);
        if (selected_room_id != mess->room_id)
        {
            GetRoom(mess->room_id)->new_messages = true;
        }
    }
    else if (auto mess = dynamic_cast<ServerJoinMessage*>(message))
    {
        Print(mess->user_name + " joined.");
    }
    else if (auto mess = dynamic_cast<ServerLeaveMessage*>(message))
    {
        Print(mess->user_name + " left.");
    }
    else if (auto mess = dynamic_cast<ServerAuthorizeMessage*>(message))
    {
        // Print( "Joined.");
        std::string m = ClientGetRoomsMessage::Serialize();
        client->Send(m);
    }
    else if (auto mess = dynamic_cast<ServerRoomListMessage*>(message))
    {
        for (int x = 0; x < mess->room_ids.size(); x++)
        {
            int id = mess->room_ids[x];
            string name = mess->room_names[x];
            GetRoom(id)->name = name;
        }
    }
    else
    {
        Print(message->to_str());
    }
}


Room* ChatWindow::GetRoom(int id, bool add_if_not_exist)
{
    auto it = rooms.find(id);
    if (it != rooms.end())
    {
        return it->second.get();
    }
    else
    {
        if (add_if_not_exist)
        {
            auto room = std::make_unique<Room>();
            rooms.emplace(id, std::move(room));
            return rooms[id].get();
        }
    }
    return nullptr;
}


Room* ChatWindow::GetActiveRoom()
{
    return ChatWindow::GetRoom(selected_room_id, true);
}
