// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include <iostream>
#include <Poco/Thread.h>
#include <Poco/Net/SocketAddress.h>
#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"

class ClientSocketHandler;
struct TextMessage;
struct Message;
struct IProtocol;
class ClientThread;
using Poco::Thread;

using Poco::BasicEvent;
using Poco::delegate;

class Client
{
public:
    Client();
    ~Client();

    void Connect(const Poco::Net::SocketAddress& address);
    void Disconnect();
    void Send(const char* str);

    void ReceiveText(const TextMessage& text_message, ClientSocketHandler* socket_handler);
    void ReceiveMessage(Message* message, ClientSocketHandler* socket_handler);

    BasicEvent<Message*> OnReceiveMessage;
private:
    Thread* thread = nullptr;
    ClientThread* client_thread = nullptr;
    IProtocol* protocol = nullptr;
};
