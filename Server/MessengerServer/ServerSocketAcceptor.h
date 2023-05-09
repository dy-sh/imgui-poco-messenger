// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <Poco/Net/SocketAcceptor.h>
#include "Protocol/IProtocol.h"

using Poco::Net::ServerSocket;

class ServerSocketAcceptor : public SocketAcceptor<ServerSocketHandler>
{
    IProtocol& protocol;
    Server& messenger;
    SocketReactor& reactor;
    
public:
    ServerSocketAcceptor(ServerSocket& socket, SocketReactor& reactor, IProtocol& protocol,Server& messenger)
        : SocketAcceptor(socket, reactor), protocol{protocol}, messenger{messenger}, reactor{reactor}
    {
    }

protected:
    ServerSocketHandler* createServiceHandler(StreamSocket& socket) override
    {
        return new ServerSocketHandler(socket, reactor,protocol,messenger);
    }
};
