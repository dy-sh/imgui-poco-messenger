// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientSocketHandler.h"

#include "Client.h"
#include "ClientUser.h"
#include "Protocol/IProtocol.h"


ClientSocketHandler::ClientSocketHandler(StreamSocket& socket, SocketReactor& reactor, IProtocol& protocol,
                                         Client* client):
    socket(socket),
    socket_stream(socket),
    reactor(reactor),
    fifo_in(BUFFER_SIZE, true),
    fifo_out(BUFFER_SIZE, true),
    protocol{&protocol},
    client{client},
    writer_worker{fifo_out, writer_queue}
{
    reactor.addEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketReadable));
    reactor.addEventHandler(socket, NObserver(*this, &ClientSocketHandler::OnSocketShutdown));

    fifo_out.readable += delegate(this, &ClientSocketHandler::OnFIFOOutReadable);
    fifo_in.writable += delegate(this, &ClientSocketHandler::OnFIFOInWritable);

    writer_thread.start(writer_worker);
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


void ClientSocketHandler::OnSocketReadable(const AutoPtr<ReadableNotification>& n)
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
            std::cerr << "ClientSocketHandler exception on reading: socket.receiveBytes == 0" << std::endl;
            client->Disconnect();
        }
    }
    catch (Poco::Exception& exc)
    {
        std::cerr << "ClientSocketHandler exception on reading [" << exc.code() << "]: " << exc.displayText() <<
            std::endl;
        client->Disconnect();
    }
}


void ClientSocketHandler::OnSocketWritable(const AutoPtr<WritableNotification>& n)
{
    try
    {
        std::string s(fifo_out.begin(), fifo_out.used());
        s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return c == '\n' || c == '\r'; }), s.end());

        std::cout << "SENDING TO SERVER: " << s << std::endl;

        socket.sendBytes(fifo_out);
    }
    catch (Poco::Exception& exc)
    {
        std::cerr << "ClientSocketHandler exception on writing [" << exc.code() << "]: " << exc.displayText() <<
            std::endl;
        client->Disconnect();
    }
}


void ClientSocketHandler::OnSocketShutdown(const AutoPtr<ShutdownNotification>& n)
{
}



void ClientSocketHandler::Send(std::string text)
{
    std::cout << "SENDING TO SERVER: " << text << std::endl;

    // working fast but cant check if socket closed and dont using mutex
    // try
    // {
    //     if (socket_stream.good())
    //     {
    //         socket_stream << text << std::endl;
    //
    //         if (!socket_stream.good())
    //         {
    //             std::cerr << "ClientSocketHandler exception on writing: socket closed." << std::endl;
    //             Stop();
    //             client->Disconnect();
    //         }
    //     }
    //     else
    //     {
    //         std::cerr << "ClientSocketHandler exception on writing: socket closed." << std::endl;
    //         Stop();
    //         client->Disconnect();
    //     }
    // }
    // catch (Poco::Exception& exc)
    // {
    //     std::cerr << "ClientSocketHandler exception on writing [" << exc.code() << "]: " << exc.displayText() <<
    //         std::endl;
    //     Stop();
    //     client->Disconnect();
    // }



    // working slow with long thread block, but safe
    //todo sent to another thread using NotificationQueue for preventing blocking UI thread
    // fifo_out.write(text.c_str(), text.size());


    // best way is to use separate thread for writing to buffer
    writer_queue.enqueueNotification(new SocketWriteNotification(fifo_out, text.c_str(), text.size()));
}


void ClientSocketHandler::Stop()
{
    writer_worker.stop();
    writer_thread.join(200);
}
