// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/NObserver.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/FIFOBuffer.h"
#include "Poco/Delegate.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>


struct ServerUser;
class MessengerServer;
struct IProtocol;
using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ReadableNotification;
using Poco::Net::WritableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::NObserver;
using Poco::AutoPtr;
using Poco::Thread;
using Poco::FIFOBuffer;
using Poco::delegate;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;

class ServerSocketHandler
{
private:
    enum
    {
        BUFFER_SIZE = 1024
    };

    StreamSocket socket;
    SocketReactor& reactor;
    FIFOBuffer fifo_in;
    FIFOBuffer fifo_out;

    IProtocol* protocol = nullptr;
    MessengerServer* messenger = nullptr;
    ServerUser* user = nullptr;

public:
    ServerSocketHandler(StreamSocket& socket, SocketReactor& reactor, IProtocol& protocol, MessengerServer& messenger);

    ~ServerSocketHandler();

    void OnFIFOOutReadable(bool& b);

    void OnFIFOInWritable(bool& b);

    void OnSocketReadable(const AutoPtr<ReadableNotification>& n);

    void OnSocketWritable(const AutoPtr<WritableNotification>& n);

    void OnSocketShutdown(const AutoPtr<ShutdownNotification>& n);

    void Send(std::string text);

    void SetUser(ServerUser* user) { this->user = user; }
    ServerUser* GetUser() { return user; }

private:
    ServerSocketHandler(StreamSocket& socket, SocketReactor& reactor): socket(socket),
                                                                       reactor(reactor),
                                                                       fifo_in(BUFFER_SIZE, true),
                                                                       fifo_out(BUFFER_SIZE, true),
                                                                       protocol(nullptr)
    {
        throw std::logic_error("Dont use MessengerServerSocketHandler deprecated constructor!");
    }


    friend SocketAcceptor<ServerSocketHandler>;
};
