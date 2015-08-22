#ifndef XML_CONFIG_H
#define XML_CONFIG_H

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


class XmlConfig {
private:
    std::string filename_;

public:
    std::string nick;
    std::string real_name;
    std::string server_ip;
    unsigned short int server_port;
    std::string server_name;
    std::string encoding;
    std::string room;
 
    XmlConfig(const std::string &filename);

private:
    // forbid CCs
    XmlConfig(const XmlConfig &);
    XmlConfig & operator= (const XmlConfig &);

public:
    void load();
    void load(const std::string &filename);
    void print_config();

};

XmlConfig::XmlConfig(const std::string &filename)
    : filename_(filename)
{
    load(filename);
}

void XmlConfig::load(const std::string &filename)
{
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(filename, pt);
    nick = pt.get("config.nick", "t3st_n1ck");
    real_name = pt.get("config.real_name", "(empty)");
    server_ip = pt.get<std::string>("config.server_ip");
    server_port = pt.get<unsigned short int>("config.server_port");
    server_name = pt.get<std::string>("config.server_name");
    encoding = pt.get<std::string>("config.encoding", "UTF-8");
    room = pt.get<std::string>("config.room");
}

void XmlConfig::print_config()
{
    std::clog << "\n\nThe CONFIG from the file " << filename_ << std::endl;
    std::clog << "nick: " << this->nick << std::endl;
    std::clog << "real name: " << this->real_name << std::endl;
    std::clog << "server_ip: " << this->server_ip << std::endl;
    std::clog << "server_port: " << this->server_port << std::endl;
    std::clog << "server_name: " << this->server_name << std::endl;
    std::clog << "encoding: " << this->encoding << std::endl;
    std::clog << "room: " << this->room << std::endl;

}


#endif  // XML_CONFIG_H
