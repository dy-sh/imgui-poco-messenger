// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include <iostream>
#include <Poco/Thread.h>
#include <Poco/Net/SocketAddress.h>


class ClientThread;
using Poco::Thread;


class Client
{
private:
    Thread* thread= nullptr;
    ClientThread* clientThread = nullptr;

public:
    ~Client()
    {
        Disconnect();
    }


    void Connect(const Poco::Net::SocketAddress& address);
    void Disconnect();
    void Send(const char* str);
};



