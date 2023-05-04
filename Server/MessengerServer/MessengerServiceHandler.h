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

class MessengerServiceHandler
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

public:
    MessengerServiceHandler(StreamSocket& socket, SocketReactor& reactor);

    ~MessengerServiceHandler();

    void onFIFOOutReadable(bool& b);

    void onFIFOInWritable(bool& b);

    void onSocketReadable(const AutoPtr<ReadableNotification>& pNf);

    void onSocketWritable(const AutoPtr<WritableNotification>& pNf);

    void onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf);
};
