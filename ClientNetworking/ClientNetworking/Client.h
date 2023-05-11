// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include <iostream>
#include <Poco/Thread.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketNotification.h>

#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"

class ClientSocketHandler;
struct ClientTextMessage;
struct Message;
struct IProtocol;
class ClientThread;
using Poco::Thread;

using Poco::BasicEvent;
using Poco::delegate;

enum class ClientState
{
    Disconnected,
    Connecting,
    Disconnecting,
    Connected
};

class Client
{
public:
    Client();
    ~Client();

    void Connect(const Poco::Net::SocketAddress& address);
    void Disconnect();
    void Send(const char* str);
    void Send(std::string str);

    BasicEvent<void> OnConnected;
    BasicEvent<void> OnDisconnected;
    BasicEvent<Message*> OnReceiveMessage;

    ClientState GetState() const { return state; }

private:
    void ReceiveText(const ClientTextMessage& text_message, ClientSocketHandler* socket_handler);
    void ReceiveMessage(Message* message, ClientSocketHandler* socket_handler);
    void OnSocketOpened(const void* sender);
    void OnSocketClosed(const void* sender);

    Thread* thread = nullptr;
    ClientThread* client_thread = nullptr;
    IProtocol* protocol = nullptr;
    ClientState state = ClientState::Disconnected;

    friend ClientSocketHandler;
};
