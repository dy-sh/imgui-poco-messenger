// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessengerServiceHandler.h"


MessengerServiceHandler::MessengerServiceHandler(StreamSocket& socket, SocketReactor& reactor):
    _socket(socket),
    _reactor(reactor),
    _fifoIn(BUFFER_SIZE, true),
    _fifoOut(BUFFER_SIZE, true)
{
    Application& app = Application::instance();
    app.logger().information("Connection from " + socket.peerAddress().toString());

    _reactor.addEventHandler(
        _socket, NObserver<MessengerServiceHandler, ReadableNotification>(*this, &MessengerServiceHandler::onSocketReadable));
    _reactor.addEventHandler(
        _socket, NObserver<MessengerServiceHandler, ShutdownNotification>(*this, &MessengerServiceHandler::onSocketShutdown));

    _fifoOut.readable += delegate(this, &MessengerServiceHandler::onFIFOOutReadable);
    _fifoIn.writable += delegate(this, &MessengerServiceHandler::onFIFOInWritable);
}


MessengerServiceHandler::~MessengerServiceHandler()
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
        _socket, NObserver<MessengerServiceHandler, ReadableNotification>(*this, &MessengerServiceHandler::onSocketReadable));
    _reactor.removeEventHandler(
        _socket, NObserver<MessengerServiceHandler, WritableNotification>(*this, &MessengerServiceHandler::onSocketWritable));
    _reactor.removeEventHandler(
        _socket, NObserver<MessengerServiceHandler, ShutdownNotification>(*this, &MessengerServiceHandler::onSocketShutdown));

    _fifoOut.readable -= delegate(this, &MessengerServiceHandler::onFIFOOutReadable);
    _fifoIn.writable -= delegate(this, &MessengerServiceHandler::onFIFOInWritable);
}


void MessengerServiceHandler::onFIFOOutReadable(bool& b)
{
    if (b)
        _reactor.addEventHandler(
            _socket, NObserver<MessengerServiceHandler, WritableNotification>(
                *this, &MessengerServiceHandler::onSocketWritable));
    else
        _reactor.removeEventHandler(
            _socket, NObserver<MessengerServiceHandler, WritableNotification>(
                *this, &MessengerServiceHandler::onSocketWritable));
}


void MessengerServiceHandler::onFIFOInWritable(bool& b)
{
    if (b)
        _reactor.addEventHandler(
            _socket, NObserver<MessengerServiceHandler, ReadableNotification>(
                *this, &MessengerServiceHandler::onSocketReadable));
    else
        _reactor.removeEventHandler(
            _socket, NObserver<MessengerServiceHandler, ReadableNotification>(
                *this, &MessengerServiceHandler::onSocketReadable));
}


void MessengerServiceHandler::onSocketReadable(const AutoPtr<ReadableNotification>& pNf)
{
    try
    {
        int len = _socket.receiveBytes(_fifoIn);
        if (len > 0)
        {
            std::string s(_fifoIn.begin(), _fifoIn.used());
            Application::instance().logger().information("RECEIVED: "+s);
            
            size_t size = _fifoOut.write(_fifoIn.buffer(), _fifoIn.used());
            _fifoIn.drain(size);
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


void MessengerServiceHandler::onSocketWritable(const AutoPtr<WritableNotification>& pNf)
{
    try
    {
        std::string s(_fifoOut.begin(), _fifoOut.used());
        Application::instance().logger().information("SENDING: "+s);

        _socket.sendBytes(_fifoOut);
    }
    catch (Poco::Exception& exc)
    {
        Application& app = Application::instance();
        app.logger().log(exc);
        delete this;
    }
}


void MessengerServiceHandler::onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    delete this;
}
