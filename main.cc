#include "client.h"
#include "xml_config.h"

#include <iostream>

int main()
{
    XmlConfig config("bynets.xml");
    config.print_config();

    while(true)
    {
        IrcClient client("bynets.xml");
        std::cerr << std::endl << std::endl;

        client.Connect(config.server_ip(), config.server_port());
        client.Register(config.nick(), config.real_name());
        try
        {
            client.Communicate();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception occured: " << e.what() << std::endl;
            continue;
        }
    }
    return 0;
}
