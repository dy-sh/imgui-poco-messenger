//
// Created by Dmitry Savosh on 18.04.2023.
//

#ifndef IMGUI_GLFW_LOGIN_WINDOW_H
#define IMGUI_GLFW_LOGIN_WINDOW_H

#include "imgui.h"

#define LOGIN_LENGTH 24
#define PASS_LENGTH 24

extern bool show_login_window;
extern bool show_main_window;

void ShowLoginWindow();

bool CheckCredentials( char login[LOGIN_LENGTH], char password[PASS_LENGTH] );

#endif // IMGUI_GLFW_LOGIN_WINDOW_H
