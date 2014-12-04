#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;



class XmlConfig {
private:
    string m_filename;

public:
    string nick;
    string real_name;
    string server_ip;
    // string server_name;

    XmlConfig(string filename);
    void load(const string &filename);

};

XmlConfig::XmlConfig(string filename)
    : m_filename(filename)
{
    clog << "CTOR" << endl;
}

void XmlConfig::load(const string &filename)
{
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(filename, pt);
    nick = pt.get("config.nick", "t3st_n1ck");
    real_name = pt.get("config.real_name", "(empty)");

}


int main()
{
    XmlConfig conf("test.xml");
    return 0;
}
