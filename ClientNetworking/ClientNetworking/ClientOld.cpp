// // Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>
//
// #include "ClientOld.h"
//
// #include "MessengerClient.h"
// #include "ClientSocketHandler.h"
// #include "ClientSocketAcceptor.h"
// #include "Protocol/SimpleProtocol.h"
//
//
// void ClientOld::initialize(Application& self)
// {
//     loadConfiguration(); // load default configuration files, if present
//     Application::initialize(self);
// }
//
//
// void ClientOld::uninitialize()
// {
//     Application::uninitialize();
// }
//
//
// void ClientOld::defineOptions(Poco::Util::OptionSet& options)
// {
//     Application::defineOptions(options);
//
//     options.addOption(
//         Option("help", "h", "display help information on command line arguments")
//         .required(false)
//         .repeatable(false));
// }
//
//
// void ClientOld::handleOption(const std::string& name, const std::string& value)
// {
//     Application::handleOption(name, value);
//
//     if (name == "help")
//         _helpRequested = true;
// }
//
//
// void ClientOld::displayHelp()
// {
//     HelpFormatter helpFormatter(options());
//     helpFormatter.setCommand(commandName());
//     helpFormatter.setUsage("OPTIONS");
//     helpFormatter.setHeader("Messenger Client");
//     helpFormatter.format(std::cout);
// }
//
//
// int ClientOld::main(const std::vector<std::string>& args)
// {
//     if (_helpRequested)
//     {
//         displayHelp();
//         return Application::EXIT_OK;
//     }
//
//     // get parameters from configuration file
//     unsigned short port = (unsigned short)config().getInt("MessengerServer.port", 9977);
//
//     SimpleProtocol protocol;
//     MessengerClient messenger;
//
//     ServerSocket svs(port);
//     SocketReactor reactor;
//     ClientSocketAcceptor acceptor(svs, reactor, protocol,messenger);
//
//
//     // run the reactor in its own thread so that we can wait for a termination request
//     Thread thread;
//     thread.start(reactor);
//
//     std::cout << "Client started" << std::endl;
//
//     // wait for CTRL-C or kill
//     // waitForTerminationRequest();
//     // Stop the SocketReactor
//     reactor.stop();
//
//     return Application::EXIT_OK;
// }