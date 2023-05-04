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

#include "MessengerServerController.h"
#include "../Protocol/IProtocol.h"


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

class MessengerServerSocketHandler
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

    IProtocol* protocol;
    MessengerServerController* controller;

public:
    MessengerServerSocketHandler(StreamSocket& socket, SocketReactor& reactor, IProtocol& protocol, MessengerServerController& controller);

    ~MessengerServerSocketHandler();

    void onFIFOOutReadable(bool& b);

    void onFIFOInWritable(bool& b);

    void onSocketReadable(const AutoPtr<ReadableNotification>& pNf);

    void onSocketWritable(const AutoPtr<WritableNotification>& pNf);

    void onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf);
private:
    MessengerServerSocketHandler(StreamSocket& socket, SocketReactor& reactor):_socket(socket),
_reactor(reactor),
_fifoIn(BUFFER_SIZE, true),
_fifoOut(BUFFER_SIZE, true),
protocol(nullptr)
    {
        throw std::logic_error("Dont use MessengerServerSocketHandler deprecated constructor!");
    }

    friend SocketAcceptor<MessengerServerSocketHandler>;
};
