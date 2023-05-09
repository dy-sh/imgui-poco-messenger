// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessageFactory.h"
#include "ClientAuthorizeMessage.h"
#include "ClientTextMessage.h"
#include "ServerTextMessage.h"

std::map<std::string, std::function<std::unique_ptr<Message>()>> MessageFactory::message_factory;

#include "ClientAuthorizeMessage.h"
REGISTER_MESSAGE(ClientAuthorizeMessage, 'a', "Client_Authorize")

#include "ClientTextMessage.h"
REGISTER_MESSAGE(ClientTextMessage, 't', "Client_Text");

#include "ClientTextMessage.h"
REGISTER_MESSAGE(ServerTextMessage, 'T', "Server_Text");