// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"

class MessengerClient;
struct IProtocol;
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
    IProtocol& protocol;
    MessengerClient& messenger;

    explicit ClientThread(IProtocol& protocol, MessengerClient& messenger, const SocketAddress& address = SocketAddress("localhost", 9977))
        : address{address}, protocol{protocol}, messenger{messenger}
    {
    }


    void run();


    void stop();


    ~ClientThread();
};