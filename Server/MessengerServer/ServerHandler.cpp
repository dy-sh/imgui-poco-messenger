// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ServerHandler.h"

#include "Protocol/SimpleProtocol.h"


ServerHandler::ServerHandler(StreamSocket& socket, SocketReactor& reactor,
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

    _reactor.addEventHandler(
        _socket, NObserver<ServerHandler, ReadableNotification>(
            *this, &ServerHandler::onSocketReadable));
    _reactor.addEventHandler(
        _socket, NObserver<ServerHandler, ShutdownNotification>(
            *this, &ServerHandler::onSocketShutdown));

    _fifoOut.readable += delegate(this, &ServerHandler::onFIFOOutReadable);
    _fifoIn.writable += delegate(this, &ServerHandler::onFIFOInWritable);
}


ServerHandler::~ServerHandler()
{
    Application& app = Application::instance();
    try
    {
        app.logger().information("Disconnecting " + _socket.peerAddress().toString());
    }
    catch (...)
    {
    }
    _reactor.removeEventHandler(
        _socket, NObserver<ServerHandler, ReadableNotification>(
            *this, &ServerHandler::onSocketReadable));
    _reactor.removeEventHandler(
        _socket, NObserver<ServerHandler, WritableNotification>(
            *this, &ServerHandler::onSocketWritable));
    _reactor.removeEventHandler(
        _socket, NObserver<ServerHandler, ShutdownNotification>(
            *this, &ServerHandler::onSocketShutdown));

    _fifoOut.readable -= delegate(this, &ServerHandler::onFIFOOutReadable);
    _fifoIn.writable -= delegate(this, &ServerHandler::onFIFOInWritable);
}


void ServerHandler::onFIFOOutReadable(bool& b)
{
    if (b)
        _reactor.addEventHandler(
            _socket, NObserver<ServerHandler, WritableNotification>(
                *this, &ServerHandler::onSocketWritable));
    else
        _reactor.removeEventHandler(
            _socket, NObserver<ServerHandler, WritableNotification>(
                *this, &ServerHandler::onSocketWritable));
}


void ServerHandler::onFIFOInWritable(bool& b)
{
    if (b)
        _reactor.addEventHandler(
            _socket, NObserver<ServerHandler, ReadableNotification>(
                *this, &ServerHandler::onSocketReadable));
    else
        _reactor.removeEventHandler(
            _socket, NObserver<ServerHandler, ReadableNotification>(
                *this, &ServerHandler::onSocketReadable));
}


void ServerHandler::onSocketReadable(const AutoPtr<ReadableNotification>& pNf)
{
    try
    {
        int len = _socket.receiveBytes(_fifoIn);
        if (len > 0)
        {
            size_t new_pos = _fifoIn.used() - len;
            std::string s(_fifoIn.begin() + new_pos, len);
            Application::instance().logger().information("RECEIVED: " + s);

            std::vector<std::string> messages = protocol->parse(_fifoIn);
            if (!messages.empty())
                messenger->receiveMessages(messages);
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


void ServerHandler::onSocketWritable(const AutoPtr<WritableNotification>& pNf)
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


void ServerHandler::onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    delete this;
}
