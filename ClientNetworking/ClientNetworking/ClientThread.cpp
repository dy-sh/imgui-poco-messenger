// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientThread.h"

#include "Client.h"
#include "ClientSocketHandler.h"


void ClientThread::run()
{
    std::cout << "Connection thread started" << std::endl;
    try
    {
        if (handler)
        {
            delete handler;
            handler = nullptr;
        }
        
        socket.connect(address);
        handler = new ClientSocketHandler(socket, reactor, protocol, client);

        OnSocketOpenedSync.set();
        OnSocketOpened();
        
        reactor.run(); // thread will be blocked here

        // stop();
    }
    catch (Poco::Exception& exc)
    {
        std::cerr << "ClientThread exception on running [" << exc.code() << "]: " << exc.displayText() << std::endl;
    }
    std::cout << "Connection thread finished" << std::endl;
    OnSocketClosed();
}


void ClientThread::stop()
{
    if (handler)
    {
        handler->Stop();
        reactor.stop();
        socket.close();
    }
}


ClientThread::~ClientThread()
{
    stop();
    delete handler;
    handler = nullptr;
    std::cout << "Connection thread destructed" << std::endl;
}
