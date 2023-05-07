//
// Created by Dmitry Savosh on 18.04.2023.
//

#include "LoginWindow.h"
#include <string>

using namespace std;


void LoginWindow::RenderContent()
{
    static char login[LOGIN_LENGTH] = "";
    static char password[PASS_LENGTH] = "";

    static bool incorrectCredentials = false; // флаг неправильного ввода

    ImGui::SetCursorPos({40, 50});
    ImGui::InputText("User", login, sizeof(login));
    ImGui::SetCursorPos({40, 100});
    ImGui::InputText("Password", password, sizeof(password), ImGuiInputTextFlags_Password);
    ImGui::SetCursorPos({100, 150});
    if (ImGui::Button("Enter", {200, 30}))
    {
        if (CheckCredentials(login, password))
        {
            incorrectCredentials = false;
            SetVisible(false);
        }
        else
        {
            incorrectCredentials = true;
        }
    }

    if (incorrectCredentials)
    {
        ImGui::SetCursorPos({100, 190});
        ImGui::Text("Invalid username or password!");
    }
}


bool LoginWindow::CheckCredentials(char login[LOGIN_LENGTH], char password[PASS_LENGTH])
{
    return login == correct_login && password == correct_password;
}
