// // Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>

struct Message;

struct MessageFactory
{
    static std::map<std::string, std::function<std::unique_ptr<Message>()>> messageFactory;
};


template <typename T>
struct MessageRegistrar
{
    MessageRegistrar(char prefix)
    {
        MessageFactory::messageFactory[T::Type] = [prefix] { return std::make_unique<T>(prefix); };
    }
};

#define REGISTER_MESSAGE(Type, Prefix) static MessageRegistrar<Type> Type##Registrar(Prefix);