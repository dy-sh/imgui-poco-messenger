// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <Poco/Net/SocketAcceptor.h>
#include "Protocol/IProtocol.h"

class ClientSocketAcceptor : public SocketAcceptor<ClientSocketHandler>
{
    IProtocol& protocol;
    MessengerClient& messenger;
    SocketReactor& reactor;
    
public:
    ClientSocketAcceptor(ServerSocket& socket, SocketReactor& reactor, IProtocol& protocol,MessengerClient& messenger)
        : SocketAcceptor<ClientSocketHandler>(socket, reactor), protocol{protocol}, messenger{messenger}, reactor{reactor}
    {
    }

protected:
    ClientSocketHandler* createServiceHandler(StreamSocket& socket) override
    {
        return new ClientSocketHandler(socket, reactor,protocol,messenger);
    }
};
