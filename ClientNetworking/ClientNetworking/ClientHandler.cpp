// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientHandler.h"

#include "MessengerClient.h"
#include "Protocol/IProtocol.h"


ClientHandler::ClientHandler(StreamSocket& socket, SocketReactor& reactor, IProtocol& protocol,
                             MessengerClient& messenger):
    socket(socket),
    stream(socket),
    reactor(reactor),
    fifo_in(BUFFER_SIZE, true),
    fifo_out(BUFFER_SIZE, true),
    protocol{&protocol},
    messenger{&messenger}
{
    reactor.addEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketReadable));
    reactor.addEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketShutdown));

    fifo_out.readable += delegate(this, &ClientHandler::OnFIFOOutReadable);
    fifo_in.writable += delegate(this, &ClientHandler::OnFIFOInWritable);
}


ClientHandler::~ClientHandler()
{
    reactor.removeEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketReadable));
    reactor.removeEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketWritable));
    reactor.removeEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketShutdown));

    fifo_out.readable -= delegate(this, &ClientHandler::OnFIFOOutReadable);
    fifo_in.writable -= delegate(this, &ClientHandler::OnFIFOInWritable);
}


void ClientHandler::OnFIFOOutReadable(bool& b)
{
    if (b)
    {
        reactor.addEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketWritable));
    }
    else
    {
        reactor.removeEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketWritable));
    }
}


void ClientHandler::OnFIFOInWritable(bool& b)
{
    if (b)
    {
        reactor.addEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketReadable));
    }
    else
    {
        reactor.removeEventHandler(socket, NObserver(*this, &ClientHandler::OnSocketReadable));
    }
}

void ClientHandler::OnSocketReadable(const AutoPtr<ReadableNotification>& pNf)
{
    try
    {
        int len = socket.receiveBytes(fifo_in);
        if (len > 0)
        {
            size_t new_pos = fifo_in.used() - len;
            std::string s(fifo_in.begin() + new_pos, len);

            std::cout << "RECEIVED FROM SERVER: " << s << std::endl;

            while (true)
            {
                auto [message, size] = protocol->ParseMessage(fifo_in.begin(), fifo_in.used());
                if (size > 0)
                {
                    // messenger->receiveMessage(message.get(), this);
                    fifo_in.drain(size);
                }
                else break;
            }
        }
        else
        {
            delete this;
        }
    }
    catch (Poco::Exception& exc)
    {
        std::cout << "Exception: " << exc.message() << std::endl;
        delete this;
    }
}



void ClientHandler::OnSocketWritable(const AutoPtr<WritableNotification>& pNf)
{
    try
    {
        std::string s(fifo_out.begin(), fifo_out.used());
        std::cout << "SENDING: " << s << std::endl;

        socket.sendBytes(fifo_out);
    }
    catch (Poco::Exception& exc)
    {
        std::cout << "Exception: " << exc.message() << std::endl;
        delete this;
    }
}


void ClientHandler::OnSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    delete this;
}

void ClientHandler::Send(const char* text)
{
    // stream << text << std::endl;

    fifo_out.write(text, std::string(text).size());
}
