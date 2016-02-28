#ifndef XML_CONFIG_H
#define XML_CONFIG_H

#include <string>

class XmlConfig
{
public:
    explicit XmlConfig(const std::string &filename);
    void load(const std::string &filename);
    void print_config() const;

    std::string nick() const;
    std::string real_name() const;
    std::string server_ip() const;
    unsigned short int server_port() const;
    std::string encoding() const;
    std::string room() const;

private:
    std::string filename_;

    std::string nick_;
    std::string real_name_;
    std::string server_ip_;
    unsigned short int server_port_;
    std::string server_name_;
    std::string encoding_;
    std::string room_;
};

#endif  // XML_CONFIG_H
