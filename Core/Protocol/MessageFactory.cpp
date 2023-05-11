// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "MessageFactory.h"

std::map<std::string, std::function<std::unique_ptr<Message>()>> MessageFactory::message_factory;

#include "Messages/ClientAuthorizeMessage.h"
REGISTER_MESSAGE(ClientAuthorizeMessage, 'a')

#include "Messages/ServerAuthorizeMessage.h"
REGISTER_MESSAGE(ServerAuthorizeMessage, 'A');


#include "Messages/ClientTextMessage.h"
REGISTER_MESSAGE(ClientTextMessage, 't');

#include "Messages/ServerTextMessage.h"
REGISTER_MESSAGE(ServerTextMessage, 'T');


#include "Messages/ServerErrorMessage.h"
REGISTER_MESSAGE(ServerErrorMessage, 'E');


#include "Messages/ServerJoinMessage.h"
REGISTER_MESSAGE(ServerJoinMessage, 'J');

#include "Messages/ServerLeaveMessage.h"
REGISTER_MESSAGE(ServerLeaveMessage, 'L');