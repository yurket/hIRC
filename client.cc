#include "client.h"


IrcClient::IrcClient(const std::string &config_filename)
{
    this->config_ = new XmlConfig(config_filename);
    this->config_->print_config();
}




int main()
{
    IrcClient client = IrcClient("irc_config.xml");
    return 0;
}
