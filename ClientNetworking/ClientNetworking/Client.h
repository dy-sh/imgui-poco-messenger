// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include <iostream>
#include <Poco/Thread.h>
#include <Poco/Net/SocketAddress.h>


struct IProtocol;
class ClientThread;
using Poco::Thread;


class Client
{
public:
    Client();

    ~Client();

    void Connect(const Poco::Net::SocketAddress& address);
    void Disconnect();
    void Send(const char* str);

private:
    Thread* thread = nullptr;
    ClientThread* client_thread = nullptr;
    IProtocol* protocol = nullptr;
};
