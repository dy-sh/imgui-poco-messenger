﻿// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Client.h"
#include "ClientSocketHandler.h"
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
    if (state == ClientState::Connecting || state == ClientState::Connected)
    {
        Disconnect();
        return;
    }

    if (client_thread)
    {
        client_thread->reactor.removeEventHandler(client_thread->socket, NObserver(*this, &Client::OnSocketShutdown));
        delete thread;
        thread = nullptr;
        // delete client_thread; - no need because called automatically from Thread
        client_thread = nullptr;
    }

    state = ClientState::Connecting;

    client_thread = new ClientThread(*protocol, this, address);
    client_thread->OnStarted += delegate(this, &Client::OnSocketStarted);

    thread = new Thread();
    thread->start(client_thread);
}


void Client::Disconnect()
{
    if (client_thread)
    {
        state = ClientState::Disconnecting;
        client_thread->stop();
    }
}


void Client::Send(const char* str)
{
    if (client_thread && client_thread->handler)
    {
        client_thread->handler->Send(str);
    }
}


void Client::ReceiveText(const ClientTextMessage& text_message, ClientSocketHandler* socket_handler)
{
}


void Client::ReceiveMessage(Message* message, ClientSocketHandler* socket_handler)
{
    OnReceiveMessage(this, message);
}


void Client::OnSocketStarted(const void* sender)
{
    client_thread->reactor.addEventHandler(client_thread->socket, NObserver(*this, &Client::OnSocketShutdown));

    state = ClientState::Connected;

    OnConnected(this);
}


void Client::OnSocketShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& n)
{
    client_thread->reactor.removeEventHandler(client_thread->socket, NObserver(*this, &Client::OnSocketShutdown));
    client_thread->stop();

    state = ClientState::Disconnected;

    OnDisconnected(this);
}
