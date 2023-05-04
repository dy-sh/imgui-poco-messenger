// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <Poco/Net/SocketAcceptor.h>

#include "Protocol/IProtocol.h"

using namespace Poco::Net;

class ServerAcceptor : public SocketAcceptor<ServerHandler>
{
    IProtocol& protocol;
    Messenger& messenger;
    SocketReactor& reactor;
    
public:
    ServerAcceptor(ServerSocket& socket, SocketReactor& reactor, IProtocol& protocol,Messenger& messenger)
        : SocketAcceptor<ServerHandler>(socket, reactor), protocol{protocol}, messenger{messenger}, reactor{reactor}
    {
    }

protected:
    ServerHandler* createServiceHandler(StreamSocket& socket) override
    {
        return new ServerHandler(socket, reactor,protocol,messenger);
    }
};
