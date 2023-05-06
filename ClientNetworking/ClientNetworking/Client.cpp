// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "Client.h"

#include "MessengerClient.h"
#include "ClientSocketHandler.h"
#include "ClientSocketAcceptor.h"
#include "Protocol/SimpleProtocol.h"


void Client::initialize(Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
}


void Client::uninitialize()
{
    ServerApplication::uninitialize();
}


void Client::defineOptions(Poco::Util::OptionSet& options)
{
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false));
}


void Client::handleOption(const std::string& name, const std::string& value)
{
    ServerApplication::handleOption(name, value);

    if (name == "help")
        _helpRequested = true;
}


void Client::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("Messenger Client");
    helpFormatter.format(std::cout);
}


int Client::main(const std::vector<std::string>& args)
{
    if (_helpRequested)
    {
        displayHelp();
        return Application::EXIT_OK;
    }

    // get parameters from configuration file
    unsigned short port = (unsigned short)config().getInt("MessengerServer.port", 9977);

    SimpleProtocol protocol;
    MessengerClient messenger;

    ServerSocket svs(port);
    SocketReactor reactor;
    ClientSocketAcceptor acceptor(svs, reactor, protocol,messenger);


    // run the reactor in its own thread so that we can wait for a termination request
    Thread thread;
    thread.start(reactor);

    std::cout << "Client started" << std::endl;

    // wait for CTRL-C or kill
    waitForTerminationRequest();
    // Stop the SocketReactor
    reactor.stop();

    return Application::EXIT_OK;
}