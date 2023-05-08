// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"
#include <iostream>


using Poco::Net::StreamSocket;
using Poco::Net::SocketStream;
using Poco::Net::SocketReactor;
using Poco::Net::ReadableNotification;
using Poco::Observer;


class ClientHandler
{
public:
    ClientHandler(StreamSocket& socket, SocketReactor& reactor) :
        socket(socket),
        stream(socket),
        reactor(reactor)
    {
        reactor.addEventHandler(
            socket, Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::OnReadable));
    }


    void OnReadable(ReadableNotification* pNf)
    {
        char buffer[1024];
        stream.getline(buffer, sizeof(buffer));
        std::cout << "RECEIVED FROM SERVER: " << buffer << std::endl;
    }


    void Send(const char* text)
    {
        stream << text << std::endl;
    }

private:
    StreamSocket socket;
    SocketStream stream;
    SocketReactor& reactor;
};