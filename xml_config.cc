#include "xml_config.h"

int main()
{
    XmlConfig conf("test.xml");
    conf.load();
    conf.print_config();

    return 0;
}
