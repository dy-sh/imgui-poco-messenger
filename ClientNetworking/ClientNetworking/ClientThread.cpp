// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientThread.h"

#include "ClientHandler.h"
#include "ClientSocketHandler.h"


void ClientThread::run()
{
    std::cerr << "Connection thread started" << std::endl;
    try
    {
        socket.connect(address);
        handler = new ClientHandler(socket, reactor);

        handler->Send("Atest1;");
        reactor.run(); // thread will be blocked here
        // socket.shutdown();
        socket.close();
        delete handler;
        handler = nullptr;
    }
    catch (Poco::Exception& e)
    {
        std::cerr << "ERROR: " << e.displayText() << std::endl;
    }
    std::cerr << "Connection thread finished" << std::endl;
}


void ClientThread::stop()
{
    reactor.stop();
    socket.close();
    delete handler;
    handler = nullptr;
}


ClientThread::~ClientThread()
{
    stop();
    std::cerr << "Connection thread destructed" << std::endl;
}
