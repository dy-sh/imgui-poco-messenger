//
// Created by Dmitry Savosh on 21.04.2023.
//

#pragma once

// #include "Console.h"
// #include "Log.h"
// #include <iomanip>
#include "../Utils/Utils.h"

// -----------------------------------------------------------------------------
//  Debug Log
// -----------------------------------------------------------------------------
// Usage:
//  include "debug.h;
//
//  LOG("Hello %d world", 123);
//  LOG(LogLevel::wrn, "Hello %d world", 123);
//
//  same as:
//  LogWindow::Add(...);
#define LOG( fmt, ... ) LogWindow::Add( fmt, ##__VA_ARGS__ )


//-----------------------------------------------------------------------------
// Debug Console
//-----------------------------------------------------------------------------
// Usage:
//  include "debug.h;
//
//  СCONSOLE("Hello %d world", 123);
//  СCONSOLE(LogLevel::wrn, "Hello %d world", 123);
//
//  same as:
//  ConsoleWindow::Add(...);
#define CONSOLE( fmt, ... ) ConsoleWindow::Add( fmt, ##__VA_ARGS__ )

enum class LogLevel
{
    inf,
    wrn,
    err
};

 ENUM_TO_STRING( LogLevel, "inf", "wrn", "err" )










