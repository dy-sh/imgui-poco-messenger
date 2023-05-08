// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Client.h"
#include "ClientHandler.h"
#include "ClientThread.h"



void Client::Connect(const SocketAddress& address)
{
    Disconnect();

    clientThread = new ClientThread();
    clientThread->address = address;
        
    thread = new Thread();
    thread->start(clientThread);
}


void Client::Disconnect()
{
    if (clientThread)
    {
        clientThread->stop();
        thread->join();
    }
    delete thread;
    thread = nullptr;
    // delete clientThread; - no need because called automatically from Thread
    clientThread = nullptr;
}


void Client::Send(const char* str)
{
    if (clientThread && clientThread->handler)
    {
        clientThread->handler->Send(str);
    }
}
