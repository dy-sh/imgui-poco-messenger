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
    ClientHandler(StreamSocket& socket, SocketReactor& reactor);
    
    void OnReadable(ReadableNotification* pNf);
    void Send(const char* text);

private:
    StreamSocket socket;
    SocketStream stream;
    SocketReactor& reactor;
};