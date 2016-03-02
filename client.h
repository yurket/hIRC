#ifndef CLIENT_H_
#define CLIENT_H_

#include "logger.h"
#include "xml_config.h"

class IrcClient
{
public:
    explicit IrcClient(const std::string &config_name);

    // forbid CC
    IrcClient(const IrcClient&) = delete;
    IrcClient& operator=(const IrcClient&) = delete;

    void Register(const std::string &nick, const std::string &real_name);
    void Connect(const std::string &server_ip, const unsigned int server_port = 6667);
    void JoinRoom(const std::string &nick, const std::string &room_name);
    void Communicate();
    void Disconnect();

private:
    static const unsigned int kRecvBufLen = 1024;
    static const unsigned int kSendBufLen = 512;
    static const unsigned int kNoFlags = 0;
    // utf-8 units can occupy 1-6 bytes
    static constexpr unsigned int kIconvBufLen = kRecvBufLen * 6;

    bool AutomaticallyHandledMsg(const char *in_buf);
    void SendPONG(const char *recv_buf);
    void SendOrDie(const std::string &send_str, bool verbose=false);
    void LogPrettifiedMessage(const std::string &message);

    int socket_;
    Logger logger_;
    XmlConfig config_;
};

#endif  // CLIENT_H_
