// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include <iostream>

#include "EchoServer/EchoServer.h"
#include "MessengerServer/MessengerServer.h"


int main(int argc, char** argv)
{
    // EchoServer app;
    MessengerServer app;
    return app.run(argc, argv);
}
