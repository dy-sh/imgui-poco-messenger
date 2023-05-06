//
// Created by Dmitry Savosh on 20.04.2023.
//

#pragma once


#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/*  -------- enum to string ---------
Usage:

enum class LogLevel
{
    inf,
    wrn,
    err
};

ENUM_TO_STRING( LogLevel, "inf", "wrn", "err" )


*/

#define ENUM_TO_STRING(EnumType, ...) \
    inline const char* ToString(EnumType value) \
    { \
        static const char* const values[] = { __VA_ARGS__ }; \
        return values[static_cast<int>(value)]; \
    }




// const char helpers
static int Stricmp( const char* s1, const char* s2 )
{
    int d;
    while( ( d = toupper( *s2 ) - toupper( *s1 ) ) == 0 && *s1 )
    {
        s1++;
        s2++;
    }
    return d;
}

static int Strnicmp( const char* s1, const char* s2, int n )
{
    int d = 0;
    while( n > 0 && ( d = toupper( *s2 ) - toupper( *s1 ) ) == 0 && *s1 )
    {
        s1++;
        s2++;
        n--;
    }
    return d;
}

static char* Strdup( const char* s )
{
    size_t len = strlen( s ) + 1;
    void* buf  = malloc( len );
    return (char*)memcpy( buf, (const void*)s, len );
}

static void Strtrim( char* s )
{
    char* str_end = s + strlen( s );
    while( str_end > s && str_end[-1] == ' ' )
        str_end--;
    *str_end = 0;
}