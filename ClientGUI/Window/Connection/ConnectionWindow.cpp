// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "ConnectionWindow.h"


#include "../../Tools/Debug.h"
#include "imgui.h"
#include "../WindowManager.h"
#include "../../Tools/Console.h"
#include "ClientNetworking/Client.h"
#include "../../Tools/Log.h"
#include "Protocol/Messages/ClientAuthorizeMessage.h"
#include "Protocol/Messages/ServerAuthorizeMessage.h"

using namespace std;
using namespace Poco::Net;


ConnectionWindow::ConnectionWindow(const std::string& title, bool visible, WindowManager* window_manager, Client* client):
    MaximizedWindow(title, visible), client{client}, window_manager{window_manager}
{
    options.no_bring_to_front = true;

    client->OnReceiveMessage += delegate(this, &ConnectionWindow::OnReceiveMessage);
    client->OnConnected += delegate(this, &ConnectionWindow::OnConnected);
    client->OnDisconnected += delegate(this, &ConnectionWindow::OnDisconnected);


    int random_num = std::rand() % 1000;


    srand(time(nullptr)); // random init


    std::snprintf(user_name, sizeof(user_name), "%s%d", "User", rand() % 10000);


    //todo temporary
    SocketAddress address = SocketAddress(server_address, server_port);
    client->Connect(address);
}


ConnectionWindow::~ConnectionWindow()
{
    client->OnReceiveMessage -= delegate(this, &ConnectionWindow::OnReceiveMessage);
    client->OnConnected -= delegate(this, &ConnectionWindow::OnConnected);
    client->OnDisconnected -= delegate(this, &ConnectionWindow::OnDisconnected);
}


void ConnectionWindow::RenderContent()
{
    if (client->GetState() == ClientState::Disconnected)
    {
        ImGui::SetCursorPosY((ImGui::GetWindowHeight() - 100) * 0.5f);

        ImGui::SetNextItemWidth(300);
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
        ImGui::InputText("Address", server_address, sizeof(server_address));

        ImGui::SetNextItemWidth(300);
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
        ImGui::InputText("User", user_name, sizeof(user_name));


        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
        if (ImGui::Button("Connect", ImVec2(200.0f, 0)))
        {
            LOG("Connecting to %s:%d ...", server_address, server_port);

            SocketAddress address = SocketAddress(server_address, server_port);
            client->Connect(address);
        }
        //Create an empty element to align elements after the button
        ImGui::Dummy(ImVec2(0, 20));
    }
    else
    {
        ImGui::SetCursorPosY((ImGui::GetWindowHeight() - 80) * 0.5f);


        if (client->GetState() == ClientState::Connecting)
        {
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 80) * 0.5f);
            ImGui::SetNextItemWidth(80);
            ImGui::Text("Connecting...");
        }
        else if (client->GetState() == ClientState::Connected)
        {
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 70) * 0.5f);
            ImGui::SetNextItemWidth(70);
            ImGui::Text("Connected");
        }
        else if (client->GetState() == ClientState::Disconnecting)
        {
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 100) * 0.5f);
            ImGui::SetNextItemWidth(100);
            ImGui::Text("Disconnecting...");
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
        if (ImGui::Button("Disconnect", ImVec2(200.0f, 0)))
        {
            if (client)
            {
                client->Disconnect();
            }
        }
        //Create an empty element to align elements after the button
        ImGui::Dummy(ImVec2(0, 20));
        
    }


    // if (ImGui::Button("Send"))
    // {
    //     if (client)
    //     {
    //         client->Send("t|Hello;");
    //     }
    // }

    // if (ImGui::Button("Add Log message"))
    // {
    //     LOG("Simple %d message example", 123);
    //     LOG(LogLevel::wrn, "wrn message example");
    //     LogWindow::Add(LogLevel::err, "err message example");
    //
    //     if (Window* log = static_cast<LogWindow*>(window_manager->GetWindowByTitle("Log")))
    //     {
    //         log->SetVisible(true);
    //     }
    // }
    //
    // if (ImGui::Button("Add Console message"))
    // {
    //     CONSOLE("Simple %d message example", 123);
    //     CONSOLE(LogLevel::wrn, "wrn message example");
    //     ConsoleWindow::Add(LogLevel::err, "err message example");
    //
    //     if (Window* log = static_cast<LogWindow*>(window_manager->GetWindowByTitle("Console")))
    //     {
    //         log->SetVisible(true);
    //     }
    // }
}


void ConnectionWindow::OnExit()
{
    if (client)
    {
        client->Disconnect();
    }
}


void ConnectionWindow::OnReceiveMessage(const void* sender, Message*& message)
{
    if (auto auth_mess = dynamic_cast<ServerAuthorizeMessage*>(message))
    {
        authorized = true;
    }
}


void ConnectionWindow::OnConnected(const void* sender)
{

    std::string mess = ClientAuthorizeMessage::Serialize(user_name);
    client->Send(mess);

    if (Window* chat_window = window_manager->GetWindowByTitle("Chat"))
    {
        this->SetVisible(false);
        chat_window->SetVisible(true);
    }
}


void ConnectionWindow::OnDisconnected(const void* sender)
{
    authorized = false;

    if (Window* chat_window = window_manager->GetWindowByTitle("Chat"))
    {
        this->SetVisible(true);
        chat_window->SetVisible(false);
    }
}
