// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientHandler.h"


ClientHandler::ClientHandler(StreamSocket& socket, SocketReactor& reactor):
    socket(socket),
    stream(socket),
    reactor(reactor)
{
    reactor.addEventHandler(
        socket, Observer<ClientHandler, ReadableNotification>(*this, &ClientHandler::OnReadable));
}


void ClientHandler::OnReadable(ReadableNotification* pNf)
{
    char buffer[1024];
    stream.getline(buffer, sizeof(buffer));
    std::cout << "RECEIVED FROM SERVER: " << buffer << std::endl;
}


void ClientHandler::Send(const char* text)
{
    stream << text << std::endl;
}
