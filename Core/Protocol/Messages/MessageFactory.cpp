// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessageFactory.h"

std::map<std::string, std::function<std::unique_ptr<Message>()>> MessageFactory::message_factory;

#include "ClientAuthorizeMessage.h"
REGISTER_MESSAGE(ClientAuthorizeMessage, 'a', "Client_Authorize")

#include "ServerAuthorizeMessage.h"
REGISTER_MESSAGE(ServerAuthorizeMessage, 'A', "Server_Authorize");


#include "ClientTextMessage.h"
REGISTER_MESSAGE(ClientTextMessage, 't', "Client_Text");

#include "ServerTextMessage.h"
REGISTER_MESSAGE(ServerTextMessage, 'T', "Server_Text");


#include "ServerErrorMessage.h"
REGISTER_MESSAGE(ServerErrorMessage, 'E', "Server_Error");