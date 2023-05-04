// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "EchoServer.h"

#include "EchoServiceHandler.h"


void EchoServer::initialize(Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
}


void EchoServer::uninitialize()
{
    ServerApplication::uninitialize();
}


void EchoServer::defineOptions(Poco::Util::OptionSet& options)
{
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false));
}


void EchoServer::handleOption(const std::string& name, const std::string& value)
{
    ServerApplication::handleOption(name, value);

    if (name == "help")
        _helpRequested = true;
}


void EchoServer::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("An echo server implemented using the Reactor and Acceptor patterns.");
    helpFormatter.format(std::cout);
}


int EchoServer::main(const std::vector<std::string>& args)
{
    if (_helpRequested)
    {
        displayHelp();
        return Application::EXIT_OK;
    }

    // get parameters from configuration file
    unsigned short port = (unsigned short)config().getInt("EchoServer.port", 9977);

    ServerSocket svs(port);
    SocketReactor reactor;
    SocketAcceptor<EchoServiceHandler> acceptor(svs, reactor);

    // run the reactor in its own thread so that we can wait for a termination request
    Thread thread;
    thread.start(reactor);

    // wait for CTRL-C or kill
    waitForTerminationRequest();
    // Stop the SocketReactor
    reactor.stop();

    return Application::EXIT_OK;
}

