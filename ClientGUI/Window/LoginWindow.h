//
// Created by Dmitry Savosh on 18.04.2023.
//

#pragma once

#include "imgui.h"

#define LOGIN_LENGTH 24
#define PASS_LENGTH 24

extern bool show_login_window;
extern bool show_main_window;

void ShowLoginWindow();

bool CheckCredentials( char login[LOGIN_LENGTH], char password[PASS_LENGTH] );


