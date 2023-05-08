// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

struct IConsoleWindow
{
    virtual void Clear() =0;
    virtual void Print(const char* fmt, ...) =0;
    virtual void ClearFilter() =0;
};
