// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessengerServerSocketHandler.h"

#include "../Protocol/MessengerSimpleProtocol.h"


MessengerServerSocketHandler::MessengerServerSocketHandler(StreamSocket& socket, SocketReactor& reactor,
                                                           IProtocol& protocol, MessengerServerController& controller):
    _socket(socket),
    _reactor(reactor),
    _fifoIn(BUFFER_SIZE, true),
    _fifoOut(BUFFER_SIZE, true),
    protocol{&protocol},
    controller{&controller}
{
    Application& app = Application::instance();
    app.logger().information("Connection from " + socket.peerAddress().toString());

    _reactor.addEventHandler(
        _socket, NObserver<MessengerServerSocketHandler, ReadableNotification>(
            *this, &MessengerServerSocketHandler::onSocketReadable));
    _reactor.addEventHandler(
        _socket, NObserver<MessengerServerSocketHandler, ShutdownNotification>(
            *this, &MessengerServerSocketHandler::onSocketShutdown));

    _fifoOut.readable += delegate(this, &MessengerServerSocketHandler::onFIFOOutReadable);
    _fifoIn.writable += delegate(this, &MessengerServerSocketHandler::onFIFOInWritable);
}


MessengerServerSocketHandler::~MessengerServerSocketHandler()
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
        _socket, NObserver<MessengerServerSocketHandler, ReadableNotification>(
            *this, &MessengerServerSocketHandler::onSocketReadable));
    _reactor.removeEventHandler(
        _socket, NObserver<MessengerServerSocketHandler, WritableNotification>(
            *this, &MessengerServerSocketHandler::onSocketWritable));
    _reactor.removeEventHandler(
        _socket, NObserver<MessengerServerSocketHandler, ShutdownNotification>(
            *this, &MessengerServerSocketHandler::onSocketShutdown));

    _fifoOut.readable -= delegate(this, &MessengerServerSocketHandler::onFIFOOutReadable);
    _fifoIn.writable -= delegate(this, &MessengerServerSocketHandler::onFIFOInWritable);
}


void MessengerServerSocketHandler::onFIFOOutReadable(bool& b)
{
    if (b)
        _reactor.addEventHandler(
            _socket, NObserver<MessengerServerSocketHandler, WritableNotification>(
                *this, &MessengerServerSocketHandler::onSocketWritable));
    else
        _reactor.removeEventHandler(
            _socket, NObserver<MessengerServerSocketHandler, WritableNotification>(
                *this, &MessengerServerSocketHandler::onSocketWritable));
}


void MessengerServerSocketHandler::onFIFOInWritable(bool& b)
{
    if (b)
        _reactor.addEventHandler(
            _socket, NObserver<MessengerServerSocketHandler, ReadableNotification>(
                *this, &MessengerServerSocketHandler::onSocketReadable));
    else
        _reactor.removeEventHandler(
            _socket, NObserver<MessengerServerSocketHandler, ReadableNotification>(
                *this, &MessengerServerSocketHandler::onSocketReadable));
}


void MessengerServerSocketHandler::onSocketReadable(const AutoPtr<ReadableNotification>& pNf)
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
                controller->receiveMessages(messages);
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


void MessengerServerSocketHandler::onSocketWritable(const AutoPtr<WritableNotification>& pNf)
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


void MessengerServerSocketHandler::onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    delete this;
}
