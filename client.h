#ifndef CLIENT_H_
#define CLIENT_H_

#include "logger.h"
#include "xml_config.h"


class IrcClient{
  // TODO: relocate class definition accordingly to Declaration Order section
  // of https://google-styleguide.googlecode.com/svn/trunk/cppguide.html
 private:
    Logger *logger_;
    int socket_;
    XmlConfig *config_;
    static const unsigned int kRecvBufLen = 1024;
    static const unsigned int kSendBufLen = 512;
    static const unsigned int kNoFlags = 0;


    bool AutomaticlyHandledMsg(const char *in_buf);
    void SendPONG(const char *recv_buf);
    void SendOrDie(const std::string &send_str, bool verbose=false);

 public:
    IrcClient();
    IrcClient(XmlConfig &conf);
    ~IrcClient();
    void Connect(const std::string server_ip="", const unsigned int server_port=6667);
    void Communicate();
    void Register(const std::string nick, const std::string real_name);
    void Disconnect();
};

#endif  // CLIENT_H_
