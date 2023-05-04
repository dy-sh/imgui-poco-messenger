// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "EchoServiceHandler.h"


EchoServiceHandler::EchoServiceHandler(StreamSocket& socket, SocketReactor& reactor):
    _socket(socket),
    _reactor(reactor),
    _fifoIn(BUFFER_SIZE, true),
    _fifoOut(BUFFER_SIZE, true)
{
    Application& app = Application::instance();
    app.logger().information("Connection from " + socket.peerAddress().toString());

    _reactor.addEventHandler(
        _socket, NObserver<EchoServiceHandler, ReadableNotification>(*this, &EchoServiceHandler::onSocketReadable));
    _reactor.addEventHandler(
        _socket, NObserver<EchoServiceHandler, ShutdownNotification>(*this, &EchoServiceHandler::onSocketShutdown));

    _fifoOut.readable += delegate(this, &EchoServiceHandler::onFIFOOutReadable);
    _fifoIn.writable += delegate(this, &EchoServiceHandler::onFIFOInWritable);
}


EchoServiceHandler::~EchoServiceHandler()
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
        _socket, NObserver<EchoServiceHandler, ReadableNotification>(*this, &EchoServiceHandler::onSocketReadable));
    _reactor.removeEventHandler(
        _socket, NObserver<EchoServiceHandler, WritableNotification>(*this, &EchoServiceHandler::onSocketWritable));
    _reactor.removeEventHandler(
        _socket, NObserver<EchoServiceHandler, ShutdownNotification>(*this, &EchoServiceHandler::onSocketShutdown));

    _fifoOut.readable -= delegate(this, &EchoServiceHandler::onFIFOOutReadable);
    _fifoIn.writable -= delegate(this, &EchoServiceHandler::onFIFOInWritable);
}


void EchoServiceHandler::onFIFOOutReadable(bool& b)
{
    if (b)
        _reactor.addEventHandler(
            _socket, NObserver<EchoServiceHandler, WritableNotification>(
                *this, &EchoServiceHandler::onSocketWritable));
    else
        _reactor.removeEventHandler(
            _socket, NObserver<EchoServiceHandler, WritableNotification>(
                *this, &EchoServiceHandler::onSocketWritable));
}


void EchoServiceHandler::onFIFOInWritable(bool& b)
{
    if (b)
        _reactor.addEventHandler(
            _socket, NObserver<EchoServiceHandler, ReadableNotification>(
                *this, &EchoServiceHandler::onSocketReadable));
    else
        _reactor.removeEventHandler(
            _socket, NObserver<EchoServiceHandler, ReadableNotification>(
                *this, &EchoServiceHandler::onSocketReadable));
}


void EchoServiceHandler::onSocketReadable(const AutoPtr<ReadableNotification>& pNf)
{
    try
    {
        int len = _socket.receiveBytes(_fifoIn);
        if (len > 0)
        {
            _fifoIn.drain(_fifoOut.write(_fifoIn.buffer(), _fifoIn.used()));
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


void EchoServiceHandler::onSocketWritable(const AutoPtr<WritableNotification>& pNf)
{
    try
    {
        // Application& app = Application::instance();
        // std::string s(_fifoOut.begin(), _fifoOut.used());
        // app.logger().information(s);

        _socket.sendBytes(_fifoOut);
    }
    catch (Poco::Exception& exc)
    {
        Application& app = Application::instance();
        app.logger().log(exc);
        delete this;
    }
}


void EchoServiceHandler::onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    delete this;
}
