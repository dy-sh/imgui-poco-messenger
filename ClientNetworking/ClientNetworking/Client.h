#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Thread.h"
#include <iostream>


using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SocketStream;
using Poco::Net::SocketReactor;
using Poco::Net::ReadableNotification;
using Poco::Thread;
using Poco::Observer;


class ClientHandler
{
public:
    ClientHandler(StreamSocket& socket, SocketReactor& reactor) :
        socket(socket),
        stream(socket),
        reactor(reactor)
    {
        reactor.addEventHandler(
            socket, Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::OnReadable));
    }


    void OnReadable(ReadableNotification* pNf)
    {
        char buffer[1024];
        stream.getline(buffer, sizeof(buffer));
        std::cout << "RECEIVED FROM SERVER: " << buffer << std::endl;
    }


    void Send(const char* text)
    {
        stream << text << std::endl;
    }

private:
    StreamSocket socket;
    SocketStream stream;
    SocketReactor& reactor;
};


class ClientThread : public Poco::Runnable
{
public:
    SocketAddress address;
    ClientHandler* handler = nullptr;
    StreamSocket socket;
    SocketReactor reactor;


    explicit ClientThread(const SocketAddress& address = SocketAddress("localhost", 9977))
        : address(address)
    {
    }


    void run()
    {
        std::cerr << "Connection thread started" << std::endl;
        try
        {
            socket.connect(address);
            handler = new ClientHandler(socket, reactor);

            handler->Send("Atest1;");
            reactor.run(); // thread will be blocked here
            // socket.shutdown();
            socket.close();
            delete handler;
            handler = nullptr;
        }
        catch (Poco::Exception& e)
        {
            std::cerr << "ERROR: " << e.displayText() << std::endl;
        }
        std::cerr << "Connection thread finished" << std::endl;
    }


    void stop()
    {
        reactor.stop();
        socket.close();
        delete handler;
        handler = nullptr;
    }


    ~ClientThread()
    {
        stop();
        std::cerr << "Connection thread destructed" << std::endl;
    }
};


class Client
{
private:
    Thread* thread= nullptr;
    ClientThread* clientThread = nullptr;

public:
    ~Client()
    {
        Disconnect();
    }


    void Connect(const SocketAddress& address)
    {
        Disconnect();

        clientThread = new ClientThread();
        clientThread->address = address;
        
        thread = new Thread();
        thread->start(clientThread);
    }


    void Disconnect()
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


    void Send(const char* str)
    {
        if (clientThread && clientThread->handler)
        {
            clientThread->handler->Send(str);
        }
    }
};
