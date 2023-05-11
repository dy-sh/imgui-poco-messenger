// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessageFactory.h"

std::map<std::string, std::function<std::unique_ptr<Message>()>> MessageFactory::message_factory;

#include "Messages/ClientAuthorizeMessage.h"
REGISTER_MESSAGE(ClientAuthorizeMessage, 'a', "Client_Authorize")

#include "Messages/ServerAuthorizeMessage.h"
REGISTER_MESSAGE(ServerAuthorizeMessage, 'A', "Server_Authorize");


#include "Messages/ClientTextMessage.h"
REGISTER_MESSAGE(ClientTextMessage, 't', "Client_Text");

#include "Messages/ServerTextMessage.h"
REGISTER_MESSAGE(ServerTextMessage, 'T', "Server_Text");


#include "Messages/ServerErrorMessage.h"
REGISTER_MESSAGE(ServerErrorMessage, 'E', "Server_Error");


#include "Messages/ServerJoinMessage.h"
REGISTER_MESSAGE(ServerJoinMessage, 'J', "Server_Join");

#include "Messages/ServerLeaveMessage.h"
REGISTER_MESSAGE(ServerLeaveMessage, 'L', "Server_Leave");