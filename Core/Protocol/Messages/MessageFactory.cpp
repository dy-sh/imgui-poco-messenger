#include "MessageFactory.h"

std::map<std::string, std::function<std::unique_ptr<Message>()>> MessageFactory::messageFactory;

#include "AuthorizeMessage.h"
REGISTER_MESSAGE(AuthorizeMessage, 'A', "Authorize")

#include "TextMessage.h"
REGISTER_MESSAGE(TextMessage, 'T', "Text");