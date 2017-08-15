#include "client.h"
#include "logger.h"
#include "xml_config.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

namespace
{

std::string ToString(const long int li)
{
    std::stringstream ss;
    ss << li;
    return ss.str();
}

} // namespace

void InitLogging()
{
    if (!Logger::Register("history", "history.txt"))
    {
        std::cerr << "Failed to register \"history\" logger." << std::endl;
    }

    if (!Logger::Register("general", "log.txt"))
    {
        std::cerr << "Failed to register \"general\" logger." << std::endl;
    }
}

int main()
{
    InitLogging();

    XmlConfig config("bynets.xml");
    config.print_config();

    Logger& logger = Logger::Get("general");
    while(true)
    {
        IrcClient client(config);
        std::cerr << std::endl << std::endl;

        try
        {
            client.Connect(config.server_ip(), config.server_port());
            client.Register(config.nick(), config.real_name());

            client.Communicate();
        }
        catch (const std::exception& e)
        {
            logger.Log("Exception occured: " + std::string(e.what()));

            std::chrono::seconds const sleep_seconds(30);
            logger.Log("Will sleep for " + ToString(sleep_seconds.count()) + " seconds before reconnecting");
            std::this_thread::sleep_for(sleep_seconds);

            continue;
        }
    }

    logger.Log("Going to exit now");
    return 0;
}
