//
// Created by Dmitry Savosh on 21.04.2023.
//

#ifndef IMGUI_APP_DEBUG_H
#define IMGUI_APP_DEBUG_H

#include "debug/console.h"
#include "debug/log.h"
#include <iomanip>

#define LOG( fmt, ... ) AppLog::Add( fmt, ##__VA_ARGS__ )
#define CONSOLE( fmt, ... ) AppConsole::Add( fmt, ##__VA_ARGS__ )

enum class LogLevel
{
    inf,
    wrn,
    err
};

ENUM_TO_STRING( LogLevel, "inf", "wrn", "err" )

template<typename T>
struct AppLogSingleton
{
private:
    AppLogSingleton()                                    = default;
    AppLogSingleton( const AppLogSingleton& )            = delete;
    AppLogSingleton& operator=( const AppLogSingleton& ) = delete;

public:
    static T* get()
    {
        static T* instance;

        if( !instance )
            instance = new T();

        return instance;
    };

    static void Add( const char* fmt, ... )
    {
        auto log = AppLogSingleton<T>::get();

        va_list args;
        va_start( args, fmt );

        int length = vsnprintf( nullptr, 0, fmt, args ); // Determine the length of the formatted string
        std::string buffer( length, '\0' );              // Allocate a buffer to store the formatted string
        vsnprintf( &buffer[0], length + 1, fmt, args );  // Format the string into the buffer
        Add( LogLevel::inf, "%s", buffer.c_str() );      // Add the formatted string to the log

        va_end( args );
    };

    static void Add( LogLevel level, const char* fmt, ... )
    {
        auto log = AppLogSingleton<T>::get();

        std::ostringstream oss;

        // append LogLevel
        oss << "[" << ToString( level ) << "] ";
        // append current time
        oss << "[" << std::fixed << std::setprecision( 2 ) << ImGui::GetTime() << "] ";
        // append message
        oss << fmt << "\n";

        std::string s = oss.str();
        fmt           = s.c_str();

        va_list args;
        va_start( args, fmt );

        int length = vsnprintf( nullptr, 0, fmt, args ); // Determine the length of the formatted string
        std::string buffer( length, '\0' );              // Allocate a buffer to store the formatted string
        vsnprintf( &buffer[0], length + 1, fmt, args );  // Format the string into the buffer
        log->Add( "%s", buffer.c_str() );                // Add the formatted string to the log

        va_end( args );
    };

    static void Draw( const char* title, bool* p_open = NULL )
    {
        auto log = AppLogSingleton<T>::get();
        log->Draw( title, p_open );
    };
};

//-----------------------------------------------------------------------------
// Debug Log
//-----------------------------------------------------------------------------
// Usage:
//  include "debug.h;
//
//  LOG("Hello %d world\n", 123);
//  AppLog::Add(LogLevel::wrn, "Hello %d world\n", 123);

struct AppLog : public AppLogSingleton<AppLogWindow>
{
};

//-----------------------------------------------------------------------------
// Debug Console
//-----------------------------------------------------------------------------
// Usage:
//  include "debug.h;
//
//  CONSOLE("Hello %d world\n", 123);
//  AppConsole::Add(LogLevel::wrn, "Hello %d world\n", 123);

struct AppConsole : public AppLogSingleton<AppConsoleWindow>
{
};

#endif // IMGUI_APP_DEBUG_H
