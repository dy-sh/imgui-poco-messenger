// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessengerServer.h"

#include "MessengerServiceHandler.h"


void MessengerServer::initialize(Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
}


void MessengerServer::uninitialize()
{
    ServerApplication::uninitialize();
}


void MessengerServer::defineOptions(Poco::Util::OptionSet& options)
{
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false));
}


void MessengerServer::handleOption(const std::string& name, const std::string& value)
{
    ServerApplication::handleOption(name, value);

    if (name == "help")
        _helpRequested = true;
}


void MessengerServer::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("Messenger Server");
    helpFormatter.format(std::cout);
}


int MessengerServer::main(const std::vector<std::string>& args)
{
    if (_helpRequested)
    {
        displayHelp();
        return Application::EXIT_OK;
    }

    // get parameters from configuration file
    unsigned short port = (unsigned short)config().getInt("MessengerServer.port", 9977);

    ServerSocket svs(port);
    SocketReactor reactor;
    SocketAcceptor<MessengerServiceHandler> acceptor(svs, reactor);

    // run the reactor in its own thread so that we can wait for a termination request
    Thread thread;
    thread.start(reactor);

    // wait for CTRL-C or kill
    waitForTerminationRequest();
    // Stop the SocketReactor
    reactor.stop();

    return Application::EXIT_OK;
}

