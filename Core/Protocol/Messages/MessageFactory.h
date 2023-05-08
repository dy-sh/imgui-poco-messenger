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
    MessageRegistrar(char prefix)
    {
        MessageFactory::message_factory[T::type] = [prefix] { return std::make_unique<T>(prefix); };
    }
};

#define REGISTER_MESSAGE(TypeName, Prefix, Name) \
const std::string TypeName::type = Name;\
static MessageRegistrar<TypeName> TypeName##Registrar(Prefix);
