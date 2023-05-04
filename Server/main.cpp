// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include <iostream>

#include "EchoServer/EchoServer.h"
#include "MessengerServer/Server.h"


int main(int argc, char** argv)
{
    // EchoServer app;
    Server app;
    return app.run(argc, argv);
}
