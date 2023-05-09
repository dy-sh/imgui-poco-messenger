// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Exception.h"
#include "Poco/FIFOBuffer.h"

struct ServerUser;
class Server;
struct IProtocol;
using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ReadableNotification;
using Poco::Net::WritableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::StreamSocket;
using Poco::AutoPtr;
using Poco::FIFOBuffer;

class ServerSocketHandler
{
private:
    enum
    {
        BUFFER_SIZE = 1024
    };

    StreamSocket socket;
    SocketReactor& reactor;
    FIFOBuffer fifo_in;
    FIFOBuffer fifo_out;

    IProtocol* protocol = nullptr;
    Server* messenger = nullptr;
    ServerUser* user = nullptr;

public:
    ServerSocketHandler(StreamSocket& socket, SocketReactor& reactor, IProtocol& protocol, Server& messenger);
    ~ServerSocketHandler();

    void OnFIFOOutReadable(bool& b);
    void OnFIFOInWritable(bool& b);
    void OnSocketReadable(const AutoPtr<ReadableNotification>& n);
    void OnSocketWritable(const AutoPtr<WritableNotification>& n);
    void OnSocketShutdown(const AutoPtr<ShutdownNotification>& n);
    void Send(std::string text);

    void SetUser(ServerUser* user) { this->user = user; }
    ServerUser* GetUser() { return user; }


    bool operator<(const ServerSocketHandler& other) const
    {
        return this < &other;
    }

private:
    ServerSocketHandler(StreamSocket& socket, SocketReactor& reactor)
        : socket(socket), reactor(reactor), fifo_in(BUFFER_SIZE, true), fifo_out(BUFFER_SIZE, true)
    {
        throw std::logic_error("Dont use MessengerServerSocketHandler deprecated constructor!");
    }


    friend SocketAcceptor<ServerSocketHandler>;
};
