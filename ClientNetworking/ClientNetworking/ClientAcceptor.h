// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <Poco/Net/SocketAcceptor.h>
#include "Protocol/IProtocol.h"

class ClientAcceptor : public SocketAcceptor<ClientSocketHandler>
{
    IProtocol& protocol;
    Messenger& messenger;
    SocketReactor& reactor;
    
public:
    ClientAcceptor(ServerSocket& socket, SocketReactor& reactor, IProtocol& protocol,Messenger& messenger)
        : SocketAcceptor<ClientSocketHandler>(socket, reactor), protocol{protocol}, messenger{messenger}, reactor{reactor}
    {
    }

protected:
    ClientSocketHandler* createServiceHandler(StreamSocket& socket) override
    {
        return new ClientSocketHandler(socket, reactor,protocol,messenger);
    }
};
