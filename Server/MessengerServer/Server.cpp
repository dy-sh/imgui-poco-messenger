// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Server.h"

#include "MessengerServer.h"
#include "ServerSocketHandler.h"
#include "ServerSocketAcceptor.h"
#include "Protocol/SimpleProtocol.h"


void Server::initialize(Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
}


void Server::uninitialize()
{
    ServerApplication::uninitialize();
}


void Server::defineOptions(OptionSet& options)
{
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false));
}


void Server::handleOption(const std::string& name, const std::string& value)
{
    ServerApplication::handleOption(name, value);

    if (name == "help")
        help_requested = true;
}


void Server::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("Messenger Server");
    helpFormatter.format(std::cout);
}


int Server::main(const std::vector<std::string>& args)
{
    if (help_requested)
    {
        displayHelp();
        return Application::EXIT_OK;
    }

    // get parameters from configuration file
    unsigned short port = (unsigned short)config().getInt("MessengerServer.port", 9977);

    SimpleProtocol protocol;
    MessengerServer messenger;

    ServerSocket svs(port);
    SocketReactor reactor;
    ServerSocketAcceptor acceptor(svs, reactor, protocol,messenger);


    // run the reactor in its own thread so that we can wait for a termination request
    Thread thread;
    thread.start(reactor);

    std::cout << "Server started" << std::endl;

    // wait for CTRL-C or kill
    waitForTerminationRequest();
    // Stop the SocketReactor
    reactor.stop();

    return Application::EXIT_OK;
}