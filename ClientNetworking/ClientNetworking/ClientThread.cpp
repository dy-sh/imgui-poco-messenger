// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientThread.h"

#include "Client.h"
#include "ClientSocketHandler.h"


void ClientThread::run()
{
    std::cout << "Connection thread started" << std::endl;
    try
    {
        socket.connect(address);
        handler = new ClientSocketHandler(socket, reactor, protocol, client);

        OnStarted.set();
        
        reactor.run(); // thread will be blocked here

        // stop();
    }
    catch (Poco::Exception& exc)
    {
        std::cout << "ClientThread exception on running [" << exc.code() << "]: " << exc.displayText() << std::endl;
    }
    std::cout << "Connection thread finished" << std::endl;
}


void ClientThread::stop()
{
    if (handler)
    {
        reactor.stop();
        socket.shutdown();
        socket.close();
        delete handler;
        handler = nullptr;
    }
}


ClientThread::~ClientThread()
{
    stop();
    std::cout << "Connection thread destructed" << std::endl;
}
