#ifndef XML_CONFIG_H
#define XML_CONFIG_H

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
    unsigned short int server_port;
    string server_name;
    string room;
 
    XmlConfig(const string &filename);

private:
    // forbid CCs
    XmlConfig(const XmlConfig &);
    XmlConfig & operator= (const XmlConfig &);

public:
    void load();
    void load(const string &filename);
    void print_config();

};

XmlConfig::XmlConfig(const string &filename)
    : m_filename(filename)
{
    load(filename);
}

void XmlConfig::load(const string &filename)
{
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(filename, pt);
    nick = pt.get("config.nick", "t3st_n1ck");
    real_name = pt.get("config.real_name", "(empty)");
    server_ip = pt.get<string>("config.server_ip");
    server_port = pt.get<unsigned short int>("config.server_port");
    server_name = pt.get<string>("config.server_name");
    room = pt.get<string>("config.room");
}

void XmlConfig::print_config()
{
    clog << "\n\nThe CONFIG from the file " << this->m_filename << endl;
    clog << "nick: " << this->nick << endl;
    clog << "real name: " << this->real_name << endl;
    clog << "server_ip: " << this->server_ip << endl;
    clog << "server_port: " << this->server_port << endl;
    clog << "server_name: " << this->server_name << endl;

}


#endif  // XML_CONFIG_H
