//
// Created by Dmitry Savosh on 18.04.2023.
//

#include "login_window.h"
#include <string>

using namespace std;

string correct_login    = "1";
string correct_password = "";

constexpr float LOGIN_WINDOW_WIDTH  = 400;
constexpr float LOGIN_WINDOW_HEIGHT = 230;

void SetLoginWindowPos()
{
    ImGui::SetNextWindowSize( { LOGIN_WINDOW_WIDTH, LOGIN_WINDOW_HEIGHT } );

    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 window_center( window_size.x / 2, window_size.y / 2 );

    ImGui::SetNextWindowPos(
        { window_center.x + ( LOGIN_WINDOW_WIDTH / 2 ), window_center.y + ( LOGIN_WINDOW_HEIGHT / 2 ) },
        ImGuiCond_Always, ImVec2( 0.5f, 0.5f ) );
}

void ShowLoginWindow()
{
    SetLoginWindowPos();

    ImGui::Begin(
        "Authorization", &show_login_window,
        ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse );

    static char login[LOGIN_LENGTH]   = "";
    static char password[PASS_LENGTH] = "";

    static bool incorrectCredentials = false; // флаг неправильного ввода

    ImGui::SetCursorPos( { 40, 50 } );
    ImGui::InputText( "Login", login, sizeof( login ) );
    ImGui::SetCursorPos( { 40, 100 } );
    ImGui::InputText( "Password", password, sizeof( password ), ImGuiInputTextFlags_Password );
    ImGui::SetCursorPos( { 100, 150 } );
    if( ImGui::Button( "Enter", { 200, 30 } ) )
    {
        if( CheckCredentials( login, password ) )
        {
            incorrectCredentials = false;
            show_main_window     = true;
            show_login_window    = false;
        }
        else
        {
            incorrectCredentials = true;
        }
    }

    if( incorrectCredentials )
    {
        ImGui::SetCursorPos( { 110, 190 } );
        ImGui::Text( "Incorrect password!" );
    }

    ImGui::End();
}

bool CheckCredentials( char login[LOGIN_LENGTH], char password[PASS_LENGTH] )
{
    return login == correct_login && password == correct_password;
}
