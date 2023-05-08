// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"

class ClientHandler;
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SocketReactor;


class ClientThread : public Poco::Runnable
{
public:
    SocketAddress address;
    ClientHandler* handler = nullptr;
    StreamSocket socket;
    SocketReactor reactor;


    explicit ClientThread(const SocketAddress& address = SocketAddress("localhost", 9977))
        : address(address)
    {
    }


    void run();


    void stop();


    ~ClientThread();
};