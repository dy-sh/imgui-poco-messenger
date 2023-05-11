// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
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
#include "Poco/NotificationQueue.h"
#include <iostream>


class Client;
struct ClientUser;
struct IProtocol;
using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ReadableNotification;
using Poco::Net::WritableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::Net::SocketStream;
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
using Poco::Notification;
using Poco::NotificationQueue;
using Poco::Runnable;


class SocketWriteNotification : public Notification
{
public:
    SocketWriteNotification(FIFOBuffer& buffer, const char* data, std::size_t size)
        : buffer(buffer), data(data, data + size)
    {
    }


    void write()
    {
        buffer.write(data.data(), data.size());
    }

private:
    FIFOBuffer& buffer;
    std::vector<char> data;
};

class SocketWriterWorker : public Runnable
{
public:
    SocketWriterWorker(FIFOBuffer& buffer, NotificationQueue& queue)
        : buffer(buffer), queue(queue)
    {
        std::cout << "Socket writing thread started" << std::endl;
    }


    ~SocketWriterWorker()
    {
        std::cout << "Socket writing thread destroyed" << std::endl;
    }


    void run()
    {
        while (!stop_flag)
        {
            Notification::Ptr pNotification(queue.waitDequeueNotification(100));
            if (pNotification)
            {
                SocketWriteNotification* pWriteNotification = dynamic_cast<SocketWriteNotification*>(pNotification.
                    get());
                if (pWriteNotification)
                {
                    pWriteNotification->write();
                }
            }
        }
        std::cout << "Socket writing thread finished" << std::endl;
    }


    void stop()
    {
        stop_flag = true;
    }

private:
    FIFOBuffer& buffer;
    NotificationQueue& queue;
    bool stop_flag = false;
};


class ClientSocketHandler
{
    constexpr static int BUFFER_SIZE = 1024;

public:
    ClientSocketHandler(StreamSocket& socket, SocketReactor& reactor, IProtocol& protocol, Client* client);
    ~ClientSocketHandler();


    void OnFIFOOutReadable(bool& b);
    void OnFIFOInWritable(bool& b);
    void OnSocketReadable(const AutoPtr<ReadableNotification>& n);
    void OnSocketWritable(const AutoPtr<WritableNotification>& n);
    void OnSocketShutdown(const AutoPtr<ShutdownNotification>& n);

    void Send(std::string text);

    void Stop();


    StreamSocket socket;
    SocketStream socket_stream;
    SocketReactor& reactor;
    FIFOBuffer fifo_in;
    FIFOBuffer fifo_out;
    IProtocol* protocol = nullptr;
    Client* client = nullptr;
    ClientUser* user = nullptr;

private:
    Poco::Mutex socket_mutex;

    // NotificationQueue writer_queue;
    // SocketWriterWorker writer_worker;
    // Thread writer_thread;
};
