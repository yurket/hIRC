#include "xml_config.h"

int main()
{
    XmlConfig conf("irc_config.xml");
    conf.print_config();

    return 0;
}
