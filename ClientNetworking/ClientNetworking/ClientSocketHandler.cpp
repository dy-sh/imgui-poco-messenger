// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientSocketHandler.h"
#include "Messenger.h"
#include "Protocol/IProtocol.h"



ClientSocketHandler::ClientSocketHandler(StreamSocket& socket, SocketReactor& reactor,
                                         IProtocol& protocol, Messenger& messenger):
    _socket(socket),
    _reactor(reactor),
    _fifoIn(BUFFER_SIZE, true),
    _fifoOut(BUFFER_SIZE, true),
    protocol{&protocol},
    messenger{&messenger}
{
    Application& app = Application::instance();
    app.logger().information("Connection from " + socket.peerAddress().toString());

    _reactor.addEventHandler(_socket, NObserver<ClientSocketHandler, ReadableNotification>(
                                 *this, &ClientSocketHandler::onSocketReadable));
    _reactor.addEventHandler(_socket, NObserver<ClientSocketHandler, ShutdownNotification>(
                                 *this, &ClientSocketHandler::onSocketShutdown));

    _fifoOut.readable += delegate(this, &ClientSocketHandler::onFIFOOutReadable);
    _fifoIn.writable += delegate(this, &ClientSocketHandler::onFIFOInWritable);
}


ClientSocketHandler::~ClientSocketHandler()
{
    Application& app = Application::instance();
    try
    {
        app.logger().information("Disconnecting " + _socket.peerAddress().toString());
    }
    catch (...)
    {
    }

    _reactor.removeEventHandler(_socket, NObserver<ClientSocketHandler, ReadableNotification>(
                                    *this, &ClientSocketHandler::onSocketReadable));
    _reactor.removeEventHandler(_socket, NObserver<ClientSocketHandler, WritableNotification>(
                                    *this, &ClientSocketHandler::onSocketWritable));
    _reactor.removeEventHandler(_socket, NObserver<ClientSocketHandler, ShutdownNotification>(
                                    *this, &ClientSocketHandler::onSocketShutdown));

    _fifoOut.readable -= delegate(this, &ClientSocketHandler::onFIFOOutReadable);
    _fifoIn.writable -= delegate(this, &ClientSocketHandler::onFIFOInWritable);
}


void ClientSocketHandler::onFIFOOutReadable(bool& b)
{
    if (b)
    {
        _reactor.addEventHandler(_socket, NObserver<ClientSocketHandler, WritableNotification>(
                                     *this, &ClientSocketHandler::onSocketWritable));
    }
    else
    {
        _reactor.removeEventHandler(_socket, NObserver<ClientSocketHandler, WritableNotification>(
                                        *this, &ClientSocketHandler::onSocketWritable));
    }
}


void ClientSocketHandler::onFIFOInWritable(bool& b)
{
    if (b)
    {
        _reactor.addEventHandler(_socket, NObserver<ClientSocketHandler, ReadableNotification>(
                                     *this, &ClientSocketHandler::onSocketReadable));
    }
    else
    {
        _reactor.removeEventHandler(_socket, NObserver<ClientSocketHandler, ReadableNotification>(
                                        *this, &ClientSocketHandler::onSocketReadable));
    }
}




void ClientSocketHandler::onSocketReadable(const AutoPtr<ReadableNotification>& pNf)
{
    try
    {
        int len = _socket.receiveBytes(_fifoIn);
        if (len > 0)
        {
            size_t new_pos = _fifoIn.used() - len;
            std::string s(_fifoIn.begin() + new_pos, len);
            Application::instance().logger().information("RECEIVED: " + s);

            while (true)
            {
                auto [message, size] = protocol->parseMessage(_fifoIn.begin(),_fifoIn.used());
                if (size>0)
                {
                    messenger->receiveMessage(message.get(), this);
                    _fifoIn.drain(size);
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


void ClientSocketHandler::onSocketWritable(const AutoPtr<WritableNotification>& pNf)
{
    try
    {
        std::string s(_fifoOut.begin(), _fifoOut.used());
        Application::instance().logger().information("SENDING: " + s);

        _socket.sendBytes(_fifoOut);
    }
    catch (Poco::Exception& exc)
    {
        Application& app = Application::instance();
        app.logger().log(exc);
        delete this;
    }
}


void ClientSocketHandler::onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    delete this;
}


void ClientSocketHandler::Send(std::string text)
{
    _fifoOut.write(text.c_str(), text.size());
}
