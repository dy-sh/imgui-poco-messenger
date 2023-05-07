//
// Created by Dmitry Savosh on 18.04.2023.
//

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "MainWindow.h"
#include "../Tools/Debug.h"
#include "imgui.h"
#include "WindowManager.h"
#include "../Tools/Console.h"
#include "ClientNetworking/Client.h"
#include "../Tools/Log.h"

using namespace std;
using namespace Poco::Net;



void MainWindow::RenderContent()
{
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    
    ImGui::InputText("Address", server_address, sizeof(server_address));

    if (ImGui::Button("Connect"))
    {
        LOG("Connecting to %s:%d ...",server_address,server_port);

        if (!client)
            client = new Client();

        SocketAddress address = SocketAddress(server_address, server_port);
        client->Connect(address);
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
            client->Send("THello;");
        }
    }

    // if (ImGui::Button("Authorization"))
    // {
    //     if (windowManager)
    //     {
    //         if (Window* loginWindow = windowManager->GetWindowByTitle("Login"))
    //         {
    //             loginWindow->ToggleVisible();
    //         }
    //     }
    // }

    // if (ImGui::Button("Add Log message"))
    // {
    //     LOG("Simple %d message example", 123);
    //     LOG(LogLevel::wrn, "wrn message example");
    //     LogWindow::Add(LogLevel::err, "err message example");
    //    
    //     if (Window* log = static_cast<LogWindow*>(windowManager->GetWindowByTitle("Log")))
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
    //     if (Window* log = static_cast<LogWindow*>(windowManager->GetWindowByTitle("Console")))
    //     {
    //         log->SetVisible(true);
    //     }
    // }
}


void MainWindow::OnExit()
{
    if (client)
    {
        client->Disconnect();
    }
}


