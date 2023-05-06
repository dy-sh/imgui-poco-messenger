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



class ClientHandler {
public:
    ClientHandler(StreamSocket& socket, SocketReactor& reactor) :
        _socket(socket),
        _stream(socket),
        _reactor(reactor)
    {
        _reactor.addEventHandler(_socket, Poco::Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::onReadable));
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


class ClientThread: public Poco::Runnable
{
public:
    ClientHandler* handler=nullptr;
    
    void run()
    {
        std::cerr << "THREAD STARTED"<< std::endl;
        try {
            Poco::Net::SocketAddress address("localhost", 9977);
            StreamSocket socket(address);
            SocketReactor reactor;
            handler = new ClientHandler(socket, reactor);
            
            
            handler->send("Atest1;");
            reactor.run(); // thread will be blocked here
        
        } catch (Poco::Exception& e) {
            std::cerr << "ERROR: " << e.displayText() << std::endl;
        }
    }

    ~ClientThread()
    {
        delete handler;
    }
};


class Client
{
public:
    void Connect()
    {
        thread.start(clientThread);
    }


    void Send(const char* str)
    {
        if (clientThread.handler)
        {
            clientThread.handler->send(str);
        }
    }

private:
    Thread thread;
    ClientThread clientThread;
};
