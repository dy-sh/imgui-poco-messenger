// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ServerSocketHandler.h"
#include "Protocol/SimpleProtocol.h"
#include "Messenger.h"
#include "Protocol/IProtocol.h"
#include "Protocol/RawMessage.h"


ServerSocketHandler::ServerSocketHandler(StreamSocket& socket, SocketReactor& reactor,
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

    _reactor.addEventHandler(_socket, NObserver<ServerSocketHandler, ReadableNotification>(
                                 *this, &ServerSocketHandler::onSocketReadable));
    _reactor.addEventHandler(_socket, NObserver<ServerSocketHandler, ShutdownNotification>(
                                 *this, &ServerSocketHandler::onSocketShutdown));

    _fifoOut.readable += delegate(this, &ServerSocketHandler::onFIFOOutReadable);
    _fifoIn.writable += delegate(this, &ServerSocketHandler::onFIFOInWritable);
}


ServerSocketHandler::~ServerSocketHandler()
{
    Application& app = Application::instance();
    try
    {
        app.logger().information("Disconnecting " + _socket.peerAddress().toString());
    }
    catch (...)
    {
    }

    _reactor.removeEventHandler(_socket, NObserver<ServerSocketHandler, ReadableNotification>(
                                    *this, &ServerSocketHandler::onSocketReadable));
    _reactor.removeEventHandler(_socket, NObserver<ServerSocketHandler, WritableNotification>(
                                    *this, &ServerSocketHandler::onSocketWritable));
    _reactor.removeEventHandler(_socket, NObserver<ServerSocketHandler, ShutdownNotification>(
                                    *this, &ServerSocketHandler::onSocketShutdown));

    _fifoOut.readable -= delegate(this, &ServerSocketHandler::onFIFOOutReadable);
    _fifoIn.writable -= delegate(this, &ServerSocketHandler::onFIFOInWritable);
}


void ServerSocketHandler::onFIFOOutReadable(bool& b)
{
    if (b)
    {
        _reactor.addEventHandler(_socket, NObserver<ServerSocketHandler, WritableNotification>(
                                     *this, &ServerSocketHandler::onSocketWritable));
    }
    else
    {
        _reactor.removeEventHandler(_socket, NObserver<ServerSocketHandler, WritableNotification>(
                                        *this, &ServerSocketHandler::onSocketWritable));
    }
}


void ServerSocketHandler::onFIFOInWritable(bool& b)
{
    if (b)
    {
        _reactor.addEventHandler(_socket, NObserver<ServerSocketHandler, ReadableNotification>(
                                     *this, &ServerSocketHandler::onSocketReadable));
    }
    else
    {
        _reactor.removeEventHandler(_socket, NObserver<ServerSocketHandler, ReadableNotification>(
                                        *this, &ServerSocketHandler::onSocketReadable));
    }
}


void ServerSocketHandler::onSocketReadable(const AutoPtr<ReadableNotification>& pNf)
{
    try
    {
        int len = _socket.receiveBytes(_fifoIn);
        if (len > 0)
        {
            size_t new_pos = _fifoIn.used() - len;
            std::string s(_fifoIn.begin() + new_pos, len);
            Application::instance().logger().information("RECEIVED: " + s);

            RawMessage message(_fifoIn);
            while (protocol->parseMessage(message))
            {
                messenger->receiveMessage(message, this);
                message.buffer.drain(message.from + message.size);
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


void ServerSocketHandler::onSocketWritable(const AutoPtr<WritableNotification>& pNf)
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


void ServerSocketHandler::onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    delete this;
}


void ServerSocketHandler::Send(std::string text)
{
    _fifoOut.write(text.c_str(), text.size());
}
