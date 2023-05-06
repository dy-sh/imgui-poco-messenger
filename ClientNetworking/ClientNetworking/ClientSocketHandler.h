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


struct User;
class MessengerClient;
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

class ClientSocketHandler
{
private:
    enum
    {
        BUFFER_SIZE = 1024
    };

    StreamSocket _socket;
    SocketReactor& _reactor;
    FIFOBuffer _fifoIn;
    FIFOBuffer _fifoOut;

    IProtocol* protocol{nullptr};
    MessengerClient* messenger{nullptr};
    User* user{nullptr};

public:
    ClientSocketHandler(StreamSocket& socket, SocketReactor& reactor, IProtocol& protocol, MessengerClient& messenger);

    ~ClientSocketHandler();

    void onFIFOOutReadable(bool& b);

    void onFIFOInWritable(bool& b);

    void onSocketReadable(const AutoPtr<ReadableNotification>& pNf);

    void onSocketWritable(const AutoPtr<WritableNotification>& pNf);

    void onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf);

    void Send(std::string text);

    void SetUser(User* user) { this->user = user; }
    User* GetUser() { return user; }

private:
    ClientSocketHandler(StreamSocket& socket, SocketReactor& reactor): _socket(socket),
                                                                       _reactor(reactor),
                                                                       _fifoIn(BUFFER_SIZE, true),
                                                                       _fifoOut(BUFFER_SIZE, true),
                                                                       protocol(nullptr)
    {
        throw std::logic_error("Dont use ClientSocketHandler deprecated constructor!");
    }


    friend SocketAcceptor<ClientSocketHandler>;
};
