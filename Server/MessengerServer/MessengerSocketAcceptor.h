// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <Poco/Net/SocketAcceptor.h>

#include "../Protocol/IProtocol.h"

using namespace Poco::Net;

class MessengerSocketAcceptor : public SocketAcceptor<MessengerServerSocketHandler>
{
    IProtocol& protocol;
    MessengerServerController& controller;
    SocketReactor& reactor;
    
public:
    MessengerSocketAcceptor(ServerSocket& socket, SocketReactor& reactor, IProtocol& protocol,MessengerServerController& controller)
        : SocketAcceptor<MessengerServerSocketHandler>(socket, reactor), protocol{protocol}, controller{controller}, reactor{reactor}
    {
    }

protected:
    MessengerServerSocketHandler* createServiceHandler(StreamSocket& socket) override
    {
        return new MessengerServerSocketHandler(socket, reactor,protocol,controller);
    }
};
