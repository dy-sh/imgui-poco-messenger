// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "MainWindow.h"
#include "../Tools/Debug.h"
#include "imgui.h"
#include "WindowManager.h"
#include "../Tools/Console.h"
#include "ClientNetworking/Client.h"
#include "../Tools/Log.h"
#include "Protocol/Messages/ServerAuthorizeMessage.h"

using namespace std;
using namespace Poco::Net;


MainWindow::MainWindow(const std::string& title, bool visible, WindowManager* window_manager, Client* client):
    MaximizedWindow(title, visible), client{client}, window_manager{window_manager}
{
    options.no_bring_to_front = true;

    client->OnReceiveMessage += delegate(this, &MainWindow::OnReceiveMessage);
    client->OnConnected += delegate(this, &MainWindow::OnConnected);
    client->OnDisconnected += delegate(this, &MainWindow::OnDisconnected);

    //todo temporary
    // SocketAddress address = SocketAddress(server_address, server_port);
    // client->Connect(address);
}


MainWindow::~MainWindow()
{
    client->OnReceiveMessage -= delegate(this, &MainWindow::OnReceiveMessage);
    client->OnConnected -= delegate(this, &MainWindow::OnConnected);
    client->OnDisconnected -= delegate(this, &MainWindow::OnDisconnected);
}


void MainWindow::RenderContent()
{
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();


    if (client->GetState() == ClientState::Disconnected)
    {
        ImGui::InputText("Address", server_address, sizeof(server_address));
        ImGui::InputText("User", user_name, sizeof(user_name));

        if (ImGui::Button("Connect"))
        {
            LOG("Connecting to %s:%d ...", server_address, server_port);

            SocketAddress address = SocketAddress(server_address, server_port);
            client->Connect(address);
        }
    }
    else
    {
        if (client->GetState() == ClientState::Connecting)
        {
            ImGui::Text("Connecting...");
        }
        else if (client->GetState() == ClientState::Connected)
        {
            ImGui::Text("Connected");
        }
        else if (client->GetState() == ClientState::Disconnecting)
        {
            ImGui::Text("Disconnecting...");
        }

        if (ImGui::Button("Disconnect"))
        {
            if (client)
            {
                client->Disconnect();
            }
        }

        if (ImGui::Button("Send"))
        {
            if (client)
            {
                client->Send("t|Hello;");
            }
        }
    }


    if (ImGui::Button("Authorization"))
    {
        if (window_manager)
        {
            if (Window* loginWindow = window_manager->GetWindowByTitle("Login"))
            {
                loginWindow->ToggleVisible();
            }
        }
    }

    if (ImGui::Button("Add Log message"))
    {
        LOG("Simple %d message example", 123);
        LOG(LogLevel::wrn, "wrn message example");
        LogWindow::Add(LogLevel::err, "err message example");

        if (Window* log = static_cast<LogWindow*>(window_manager->GetWindowByTitle("Log")))
        {
            log->SetVisible(true);
        }
    }

    if (ImGui::Button("Add Console message"))
    {
        CONSOLE("Simple %d message example", 123);
        CONSOLE(LogLevel::wrn, "wrn message example");
        ConsoleWindow::Add(LogLevel::err, "err message example");

        if (Window* log = static_cast<LogWindow*>(window_manager->GetWindowByTitle("Console")))
        {
            log->SetVisible(true);
        }
    }
}


void MainWindow::OnExit()
{
    if (client)
    {
        client->Disconnect();
    }
}


void MainWindow::OnReceiveMessage(const void* sender, Message*& message)
{
    if (auto auth_mess = dynamic_cast<ServerAuthorizeMessage*>(message))
    {
        authorized = true;
    }
}


void MainWindow::OnConnected(const void* sender)
{
    std::string mess = Poco::format("a|%s;", std::string(user_name));
    client->Send(mess.c_str());

    if (Window* chat_window = window_manager->GetWindowByTitle("Chat"))
    {
        chat_window->SetVisible(true);
    }
}


void MainWindow::OnDisconnected(const void* sender)
{
    authorized = false;

    if (Window* chat_window = window_manager->GetWindowByTitle("Chat"))
    {
        chat_window->SetVisible(false);
    }
}
