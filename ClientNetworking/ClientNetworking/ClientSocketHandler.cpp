// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientSocketHandler.h"
#include "MessengerClient.h"
#include "Protocol/IProtocol.h"


ClientSocketHandler::ClientSocketHandler(StreamSocket& socket, SocketReactor& reactor,
                                         IProtocol& protocol, MessengerClient& messenger):
    socket(socket),
    reactor(reactor),
    fifo_in(BUFFER_SIZE, true),
    fifo_out(BUFFER_SIZE, true),
    protocol{&protocol},
    messenger{&messenger}
{
    Application& app = Application::instance();
    app.logger().information("Connection from " + socket.peerAddress().toString());

    reactor.addEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketReadable));
    reactor.addEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketShutdown));

    fifo_out.readable += delegate(this, &ClientSocketHandler::OnFIFOOutReadable);
    fifo_in.writable += delegate(this, &ClientSocketHandler::OnFIFOInWritable);
}


ClientSocketHandler::~ClientSocketHandler()
{
    Application& app = Application::instance();
    try
    {
        app.logger().information("Disconnecting " + socket.peerAddress().toString());
    }
    catch (...)
    {
    }

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
            Application::instance().logger().information("RECEIVED: " + s);

            while (true)
            {
                auto [message, size] = protocol->ParseMessage(fifo_in.begin(), fifo_in.used());
                if (size > 0)
                {
                    messenger->receiveMessage(message.get(), this);
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


void ClientSocketHandler::OnSocketWritable(const AutoPtr<WritableNotification>& pNf)
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


void ClientSocketHandler::OnSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    delete this;
}


void ClientSocketHandler::Send(std::string text)
{
    fifo_out.write(text.c_str(), text.size());
}
