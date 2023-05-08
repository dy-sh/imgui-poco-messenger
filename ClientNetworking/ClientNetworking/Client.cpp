// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Client.h"
#include "ClientHandler.h"
#include "ClientThread.h"
#include "Protocol/SimpleProtocol.h"


Client::Client()
{
    protocol = new SimpleProtocol();
}


Client::~Client()
{
    Disconnect();
    delete protocol;
}


void Client::Connect(const SocketAddress& address)
{
    Disconnect();

    client_thread = new ClientThread();
    client_thread->address = address;
        
    thread = new Thread();
    thread->start(client_thread);
}


void Client::Disconnect()
{
    if (client_thread)
    {
        client_thread->stop();
        thread->join();
    }
    delete thread;
    thread = nullptr;
    // delete clientThread; - no need because called automatically from Thread
    client_thread = nullptr;
}


void Client::Send(const char* str)
{
    if (client_thread && client_thread->handler)
    {
        client_thread->handler->Send(str);
    }
}
