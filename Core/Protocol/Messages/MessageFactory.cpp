#include "MessageFactory.h"

std::map<std::string, std::function<std::unique_ptr<Message>()>> MessageFactory::messageFactory;

#include "AuthorizeMessage.h"
const std::string AuthorizeMessage::Type = "Authorize";
REGISTER_MESSAGE(AuthorizeMessage, 'A')

#include "TextMessage.h"
const std::string TextMessage::Type = "Text";
REGISTER_MESSAGE(TextMessage, 'T');