// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ServerSocketHandler.h"

#include <Poco/Delegate.h>
#include <Poco/NObserver.h>
#include <Poco/Util/Application.h>

#include "MessengerServer.h"
#include "Protocol/IProtocol.h"

using Poco::Util::Application;
using Poco::NObserver;
using Poco::delegate;

ServerSocketHandler::ServerSocketHandler(StreamSocket& socket, SocketReactor& reactor,
                                         IProtocol& protocol, MessengerServer& messenger):
    socket(socket),
    reactor(reactor),
    fifo_in(BUFFER_SIZE, true),
    fifo_out(BUFFER_SIZE, true),
    protocol{&protocol},
    messenger{&messenger}
{
    Application& app = Application::instance();
    app.logger().information("Connection from " + socket.peerAddress().toString());

    reactor.addEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketReadable));
    reactor.addEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketShutdown));

    fifo_out.readable += delegate(this, &ServerSocketHandler::OnFIFOOutReadable);
    fifo_in.writable += delegate(this, &ServerSocketHandler::OnFIFOInWritable);
}


ServerSocketHandler::~ServerSocketHandler()
{
    Application& app = Application::instance();
    try
    {
        app.logger().information("Disconnecting " + socket.peerAddress().toString());
    }
    catch (...)
    {
    }

    reactor.removeEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketReadable));
    reactor.removeEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketWritable));
    reactor.removeEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketShutdown));

    fifo_out.readable -= delegate(this, &ServerSocketHandler::OnFIFOOutReadable);
    fifo_in.writable -= delegate(this, &ServerSocketHandler::OnFIFOInWritable);
}


void ServerSocketHandler::OnFIFOOutReadable(bool& b)
{
    if (b)
    {
        reactor.addEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketWritable));
    }
    else
    {
        reactor.removeEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketWritable));
    }
}


void ServerSocketHandler::OnFIFOInWritable(bool& b)
{
    if (b)
    {
        reactor.addEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketReadable));
    }
    else
    {
        reactor.removeEventHandler(socket, NObserver(*this, &ServerSocketHandler::OnSocketReadable));
    }
}


void ServerSocketHandler::OnSocketReadable(const AutoPtr<ReadableNotification>& n)
{
    try
    {
        int len = socket.receiveBytes(fifo_in);
        if (len > 0)
        {
            size_t new_pos = fifo_in.used() - len;
            std::string s(fifo_in.begin() + new_pos, len);
            Application::instance().logger().information("RECEIVED: " + s);

            while (true)
            {
                auto [message, size] = protocol->ParseMessage(fifo_in.begin(), fifo_in.used());
                if (size > 0)
                {
                    messenger->ReceiveMessage(message.get(), this);
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
        Application& app = Application::instance();
        app.logger().log(exc);
        delete this;
    }
}


void ServerSocketHandler::OnSocketWritable(const AutoPtr<WritableNotification>& n)
{
    try
    {
        std::string s(fifo_out.begin(), fifo_out.used());
        Application::instance().logger().information("SENDING: " + s);

        socket.sendBytes(fifo_out);
    }
    catch (Poco::Exception& exc)
    {
        Application& app = Application::instance();
        app.logger().log(exc);
        delete this;
    }
}


void ServerSocketHandler::OnSocketShutdown(const AutoPtr<ShutdownNotification>& n)
{
    delete this;
}


void ServerSocketHandler::Send(std::string text)
{
    fifo_out.write(text.c_str(), text.size());
}
