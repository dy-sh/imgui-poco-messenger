// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#include "SimpleProtocol.h"

#include <stdexcept>

#include "Messages/InvalidMessage.h"
#include "Messages/MessageFactory.h"


std::pair<std::unique_ptr<Message>, size_t> SimpleProtocol::ParseMessage(const char* buffer, size_t buffer_size)
{
    size_t from = 0;
    size_t size = 0;

    // find start (skip all '\r' and '\n')
    for (size_t i = 0; i < buffer_size; i++)
    {
        if (buffer[i] == '\r' || buffer[i] == '\n' || buffer[i] == ' ' || buffer[i] == DELIMITER)
        {
            from++;
        }
        else
        {
            break;
        }
    }

    // find end
    for (size_t i = from; i < buffer_size; ++i)
    {
        if (buffer[i] == DELIMITER)
        {
            size = i + 1 - from;
            break;
        }
    }
    if (size == 0)
    {
        return {nullptr, 0}; // end not found
    }

    //check parsers
    if (MessageFactory::message_factory.empty())
    {
        throw std::logic_error("No message types registered. Use REGISTER_MESSAGE macro to register message types.");
    }

    // parse message
    for (auto& [type, messageCreator] : MessageFactory::message_factory)
    {
        if (messageCreator()->Matches(buffer, from, size)) //todo don't create new instances each time to match
        {
            auto message = std::move(messageCreator());
            if (message->Parse(buffer, from, size))
            {
                return {std::move(message), from + size};
            }
        }
    }

    auto message = std::make_unique<InvalidMessage>(buffer, from, size);
    return {std::move(message), from + size};
}
