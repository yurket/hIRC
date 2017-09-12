#ifndef CLIENT_H_
#define CLIENT_H_

#include "logger.h"
#include "xml_config.h"


namespace
{

const std::size_t kUnicodePoinMaxSize = 6;

} // namespace


class IrcClient
{
public:
    explicit IrcClient(XmlConfig xml_config);
    ~IrcClient();

    // forbid CC
    IrcClient(const IrcClient&) = delete;
    IrcClient& operator=(const IrcClient&) = delete;

    void Register(const std::string& nick, const std::string& real_name);
    void Connect(const std::string& server_ip, const unsigned short server_port = 6667);
    void JoinRoom(const std::string& nick, const std::string& room_name);
    void Communicate();
    void Disconnect();

private:
    static const std::size_t  kRecvBufLen = 1024;
    static const std::size_t kSendBufLen = 512;
    static const std::size_t kNoFlags = 0;
    static constexpr std::size_t kIconvBufLen = kRecvBufLen * kUnicodePoinMaxSize;

    bool IsAutomaticallyHandledMsg(const char* in_buf);
    void SendPONG(const char* recv_buf);
    void SendOrDie(const std::string& send_str);

    int socket_;
    Logger& messages_logger_;
    Logger& logger_;
    XmlConfig config_;
};

#endif  // CLIENT_H_
