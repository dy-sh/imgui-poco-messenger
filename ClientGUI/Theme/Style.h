// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once


struct Style
{
    virtual ~Style() = default;
    virtual void ApplyStyle(ImVec4& bg_color, ImGuiStyle* dst = nullptr)=0;
};
