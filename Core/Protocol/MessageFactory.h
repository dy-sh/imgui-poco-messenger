// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>

struct Message;

struct MessageFactory
{
    static std::map<std::string, std::function<std::unique_ptr<Message>()>> message_factory;
};


template <typename T>
struct MessageRegistrar
{
    MessageRegistrar()
    {
        MessageFactory::message_factory[T::type] = []{ return std::make_unique<T>(); };
    }
};

#define REGISTER_MESSAGE(TypeName, Prefix) \
const std::string TypeName::type = #TypeName;\
const char TypeName::prefix = Prefix;\
static MessageRegistrar<TypeName> TypeName##Registrar{};

