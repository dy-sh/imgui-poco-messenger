#include "MessengerServerController.h"

#include <iostream>


void MessengerServerController::receiveMessages(std::vector<std::string> messages)
{
    for (auto m : messages)
    {
        std::cout << "Received message: " << m << std::endl;
    }
}
