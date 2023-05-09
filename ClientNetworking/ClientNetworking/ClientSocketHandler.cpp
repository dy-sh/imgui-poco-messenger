// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientSocketHandler.h"

#include "Client.h"
#include "Protocol/IProtocol.h"


ClientSocketHandler::ClientSocketHandler(StreamSocket& socket, SocketReactor& reactor, IProtocol& protocol,
                             Client* client):
    socket(socket),
    stream(socket),
    reactor(reactor),
    fifo_in(BUFFER_SIZE, true),
    fifo_out(BUFFER_SIZE, true),
    protocol{&protocol},
    client{client}
{
    reactor.addEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketReadable));
    reactor.addEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketShutdown));

    fifo_out.readable += delegate(this, &ClientSocketHandler::OnFIFOOutReadable);
    fifo_in.writable += delegate(this, &ClientSocketHandler::OnFIFOInWritable);

    client->OnConnected(this);
}


ClientSocketHandler::~ClientSocketHandler()
{
    reactor.removeEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketReadable));
    reactor.removeEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketWritable));
    reactor.removeEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketShutdown));

    fifo_out.readable -= delegate(this, &ClientSocketHandler::OnFIFOOutReadable);
    fifo_in.writable -= delegate(this, &ClientSocketHandler::OnFIFOInWritable);
}


void ClientSocketHandler::OnFIFOOutReadable(bool& b)
{
    if (b)
    {
        reactor.addEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketWritable));
    }
    else
    {
        reactor.removeEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketWritable));
    }
}


void ClientSocketHandler::OnFIFOInWritable(bool& b)
{
    if (b)
    {
        reactor.addEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketReadable));
    }
    else
    {
        reactor.removeEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketReadable));
    }
}

void ClientSocketHandler::OnSocketReadable(const AutoPtr<ReadableNotification>& pNf)
{
    try
    {
        int len = socket.receiveBytes(fifo_in);
        if (len > 0)
        {
            size_t new_pos = fifo_in.used() - len;
            std::string s(fifo_in.begin() + new_pos, len);
            s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return c == '\n' || c == '\r'; }), s.end());
            std::cout << "RECEIVED FROM SERVER: " << s << std::endl;

            while (true)
            {
                auto [message, size] = protocol->ParseMessage(fifo_in.begin(), fifo_in.used());
                if (size > 0)
                {
                    client->ReceiveMessage(message.get(), this);
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



void ClientSocketHandler::OnSocketWritable(const AutoPtr<WritableNotification>& pNf)
{
    try
    {
        std::string s(fifo_out.begin(), fifo_out.used());
        s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return c == '\n' || c == '\r'; }), s.end());
        std::cout << "SENDING: " << s << std::endl;

        socket.sendBytes(fifo_out);
    }
    catch (Poco::Exception& exc)
    {
        std::cout << "Exception: " << exc.message() << std::endl;
        delete this;
    }
}


void ClientSocketHandler::OnSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    client->OnDisconnected(this);
    delete this;
}

void ClientSocketHandler::Send(const char* text)
{
    // stream << text << std::endl;

    fifo_out.write(text, std::string(text).size());
}
