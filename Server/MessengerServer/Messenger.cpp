#include "Messenger.h"

#include <iostream>


void Messenger::receiveMessages(std::vector<std::string> messages)
{
    for (auto m : messages)
    {
        std::cout << "Received message: " << m << std::endl;
    }
}
