#include "xml_config.hpp"

int main()
{
    XmlConfig conf("test.xml");
    conf.load();
    conf.print_config();

    return 0;
}
