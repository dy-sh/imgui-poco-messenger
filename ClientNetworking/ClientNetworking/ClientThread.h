// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"

class Client;
struct IProtocol;
class ClientSocketHandler;
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SocketReactor;
using Poco::Event;
using Poco::BasicEvent;


class ClientThread : public Poco::Runnable
{
public:
    SocketAddress address;
    ClientSocketHandler* handler = nullptr;
    StreamSocket socket;
    SocketReactor reactor;
    IProtocol& protocol;
    Client* client;
    Event OnStartedSync;
    BasicEvent<void> OnStarted;

    explicit ClientThread(IProtocol& protocol, Client* client, const SocketAddress& address = SocketAddress("localhost", 9977))
        : address{address}, protocol{protocol}, client{client}
    {
    }


    void run();


    void stop();


    ~ClientThread();
};