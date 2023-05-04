// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <Poco/Net/SocketAcceptor.h>

#include "Protocol/IProtocol.h"

using namespace Poco::Net;

class ServerAcceptor : public SocketAcceptor<ServerSocketHandler>
{
    IProtocol& protocol;
    Messenger& messenger;
    SocketReactor& reactor;
    
public:
    ServerAcceptor(ServerSocket& socket, SocketReactor& reactor, IProtocol& protocol,Messenger& messenger)
        : SocketAcceptor<ServerSocketHandler>(socket, reactor), protocol{protocol}, messenger{messenger}, reactor{reactor}
    {
    }

protected:
    ServerSocketHandler* createServiceHandler(StreamSocket& socket) override
    {
        return new ServerSocketHandler(socket, reactor,protocol,messenger);
    }
};
