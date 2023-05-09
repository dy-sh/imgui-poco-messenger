// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "App.h"

#include <Poco/Util/HelpFormatter.h>
#include "Protocol/SimpleProtocol.h"

#include "Server.h"
#include "ServerSocketHandler.h"
#include "ServerSocketAcceptor.h"

using Poco::Util::Option;
using Poco::Util::HelpFormatter;
using Poco::Thread;

void App::initialize(Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
}


void App::uninitialize()
{
    ServerApplication::uninitialize();
}


void App::defineOptions(OptionSet& options)
{
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false));
}


void App::handleOption(const std::string& name, const std::string& value)
{
    ServerApplication::handleOption(name, value);

    if (name == "help")
        help_requested = true;
}


void App::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("Messenger Server");
    helpFormatter.format(std::cout);
}


int App::main(const std::vector<std::string>& args)
{
    if (help_requested)
    {
        displayHelp();
        return Application::EXIT_OK;
    }

    // get parameters from configuration file
    unsigned short port = (unsigned short)config().getInt("MessengerServer.port", 9977);

    SimpleProtocol protocol;
    Server messenger;

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