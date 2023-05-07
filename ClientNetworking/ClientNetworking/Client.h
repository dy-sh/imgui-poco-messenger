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


class ClientHandler
{
public:
    ClientHandler(StreamSocket& socket, SocketReactor& reactor) :
        _socket(socket),
        _stream(socket),
        _reactor(reactor)
    {
        _reactor.addEventHandler(
            _socket, Poco::Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::onReadable));
    }


    void onReadable(ReadableNotification* pNf)
    {
        char buffer[1024];
        _stream.getline(buffer, sizeof(buffer));
        std::cout << "RECEIVED FROM SERVER: " << buffer << std::endl;
    }


    void send(const char* text)
    {
        _stream << text << std::endl;
    }

private:
    StreamSocket _socket;
    Poco::Net::SocketStream _stream;
    SocketReactor& _reactor;
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

            handler->send("Atest1;");
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
            clientThread->handler->send(str);
        }
    }
};
