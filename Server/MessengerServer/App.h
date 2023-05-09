// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Poco/Net/SocketReactor.h"
#include "Poco/Util/ServerApplication.h"

using Poco::Util::OptionSet;


class App : public Poco::Util::ServerApplication
{
public:
    App(): help_requested(false)
    {
    }


    ~App()
    {
    }

protected:
    void initialize(Application& self) override;

    void uninitialize() override;

    void defineOptions(OptionSet& options) override;

    void handleOption(const std::string& name, const std::string& value) override;

    void displayHelp();

    int main(const std::vector<std::string>& args) override;

private:
    bool help_requested;
};

