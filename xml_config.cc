#include "xml_config.h"

#include <iostream>
#include <stdexcept>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

XmlConfig::XmlConfig(const std::string& filename)
    : filename_(filename)
{
    load(filename_);
}

void XmlConfig::load(const std::string& filename)
{
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(filename_, pt);
    nick_ = pt.get("config.nick", "t3st_n1ck");
    real_name_ = pt.get("config.real_name", "(empty)");
    server_ip_ = pt.get<std::string>("config.server_ip");
    server_port_ = pt.get<unsigned short int>("config.server_port");
    server_name_ = pt.get<std::string>("config.server_name");
    room_ = pt.get<std::string>("config.room");

    try
    {
        encoding_ = pt.get<std::string>("config.encoding");
    }
    catch (boost::property_tree::ptree_bad_path const& e)
    {
        std::clog << "[W] settings default encoding to \"UTF-8\"" << std::endl;
        encoding_ = "UTF-8";
    }
}

void XmlConfig::print_config() const
{
    std::clog << "\n\nThe CONFIG from the file " << filename_ << std::endl;
    std::clog << "nick: " << nick_ << std::endl;
    std::clog << "real name: " << real_name_ << std::endl;
    std::clog << "server_ip: " << server_ip_ << std::endl;
    std::clog << "server_port: " << server_port_ << std::endl;
    std::clog << "server_name: " << server_name_ << std::endl;
    std::clog << "encoding: " << encoding_ << std::endl;
    std::clog << "room: " << room_ << std::endl;
}

std::string XmlConfig::nick() const
{
    return nick_;
}

std::string XmlConfig::real_name() const
{
    return real_name_;
}

std::string XmlConfig::server_ip() const
{
    return server_ip_;
}

unsigned short int XmlConfig::server_port() const
{
    return server_port_;
}

std::string XmlConfig::encoding() const
{
    return encoding_;
}

std::string XmlConfig::room() const
{
    return room_;
}
