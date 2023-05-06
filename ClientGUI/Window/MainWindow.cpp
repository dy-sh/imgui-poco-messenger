//
// Created by Dmitry Savosh on 18.04.2023.
//

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include <iostream>

#include "MainWindow.h"
#include "../Debug/Debug.h"
#include "imgui.h"
// #include "ClientNetworking/Client.h"

using namespace std;  
using namespace Poco::Net;  



void MainWindow::RenderContent()
{
    // if( maximize_main_window )
    // {
    //     MaximizeMainWindow();
    //     ImGui::Begin(
    //         "Main", &show_main_window,
    //         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus );
    // }
    // else
    // {
    //     ImGui::SetNextWindowSize( ImVec2( 400, 500 ), ImGuiCond_FirstUseEver );
    //     ImGui::Begin( "Main", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus );
    // }

    static int counter = 0;
    ImGui::Text( "Alt + C - Console" );
    ImGui::Text( "Alt + X - Log" );
    ImGui::Text( "Esc - Quit" );

    
    if( ImGui::Button( "Connect" ) )
    {
        // client.Connect();
    }
    if( ImGui::Button( "Send" ) )
    {
        // client.Send("THello;");
    }

    if( ImGui::Button( "Simple Button" ) )
    {
        counter++;
    }
    ImGui::SameLine();
    ImGui::Text( "counter = %d", counter );

    if( ImGui::Button( "Authorization" ) )
    {
        // show_login_window = true;
    }

    if( ImGui::Button( "Add Log message" ) )
    {
        LOG( "Simple %d message example", 123 );
        LOG(LogLevel::wrn, "wrn message example" );
        AppLog::Add(LogLevel::err, "err message example" );

        // show_app_log = true;
    }

    if( ImGui::Button( "Add Console message" ) )
    {
        CONSOLE( "Simple %d message example", 123 );
        CONSOLE(LogLevel::wrn, "wrn message example" );
        AppConsole::Add(LogLevel::err, "err message example" );

        // show_app_console = true;
    }

    // ImGui::End();

    // if( maximize_main_window )
    //     FinishMaximizeMainWindow();
}

