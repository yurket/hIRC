#ifndef CLIENT_H_
#define CLIENT_H_

#include "xml_config.h"


class IrcClient{
    int socket_;
    XmlConfig *config_;
    
public:
    IrcClient();
    IrcClient(const std::string &config_filename);
    void Connect(const std::string server_ip);
    void Register(const std::string nick, const std::string real_name);
    void Disconnect();
};

#endif  // CLIENT_H_
