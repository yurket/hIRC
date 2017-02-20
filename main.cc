#include "client.h"
#include "xml_config.h"

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    XmlConfig config("bynets.xml");
    config.print_config();

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
            std::cerr << "Exception occured: " << e.what() << std::endl;

            std::chrono::seconds const sleep_seconds(30);
            std::cerr << "Will sleep for " << sleep_seconds.count() << " seconds before reconnecting" << std:: endl;
            std::this_thread::sleep_for(sleep_seconds);

            continue;
        }
    }
    return 0;
}
