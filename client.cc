#include "client.h"
#include "libiconv_wrapper.h"
#include "message.h"

#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <vector>

namespace
{

const int kRecvConnectionClosedByPeer = 0;


// TODO: write tests!
std::vector<std::string> SplitOnLines(char const* text_of_chars)
{
    std::string const text(text_of_chars);
    std::vector<std::string> result;

    std::size_t last_pos = 0;
    for (std::size_t endline_pos = text.find('\n');
         endline_pos != std::string::npos;
         endline_pos = text.find('\n', endline_pos + 1))
    {
        result.push_back(text.substr(last_pos, (endline_pos-last_pos+1)));
        last_pos = endline_pos + 1;
    }
    return result;
}

bool IfPingRequest(const char* recv_buf)
{
    assert(recv_buf != NULL);
    if (recv_buf == NULL)
    {
        return false;
    }

    const std::string kPingString = std::string("PING :");

    std::string received_str = std::string(recv_buf);
    if (received_str.find(kPingString) == 0)
    {
        return true;
    }
    return false;
}

bool IfErrorCommand(const char* recv_buf)
{
    assert(recv_buf != NULL);
    if (recv_buf == NULL)
    {
        return false;
    }

    const std::string kErrorString = std::string("ERROR :");

    std::string received_str = std::string(recv_buf);
    if (received_str.find(kErrorString) == 0)
    {
        return true;
    }
    return false;
}

void CloseVerbose(int fd)
{
    if (close(fd) == -1)
    {
        perror("close error");
    }
}

} // namespace


IrcClient::IrcClient(XmlConfig xml_config)
    : socket_(-1)
    , messages_logger_("history.txt")
    , logger_("log.txt")
    , config_(std::move(xml_config))
{
    // disable logging for not to log greeting messages from IRC server
    logger_.DisableLogging();
}

IrcClient::~IrcClient()
{
    if (socket_ != -1)
    {
        CloseVerbose(socket_);
    }
}

void IrcClient::Connect(const std::string& server_ip, const unsigned short server_port)
{
    logger_.Log("Trying to connect to server " + server_ip);

    int res = 0;
    struct sockaddr_in addr;

    socket_ = socket(AF_INET, SOCK_STREAM, kNoFlags);
    if (socket_ == -1)
    {
        perror("Can't create socket");
        throw std::runtime_error("socket error");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    res = inet_aton(server_ip.c_str(), &addr.sin_addr);
    if (res != 1)
    {
        perror("inet_aton error");
        throw std::runtime_error("inet_aton error");
    }

    res = connect(socket_, (const struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        perror("connect error");
        throw std::runtime_error("connect error");
    }
    logger_.Log("Successfully connected");
}

void IrcClient::Register(const std::string& nick, const std::string& real_name)
{
    std::string send_str = "PASS 12345\r\n";
    SendOrDie(send_str);

    send_str = "NICK " + nick + "\r\n";
    SendOrDie(send_str);

    send_str = "USER " + nick + " 0 * :" + real_name + "\r\n";
    SendOrDie(send_str);

    logger_.Log("Successfully registered with name " + nick);
}

void IrcClient::JoinRoom(const std::string& nick, const std::string& room_name)
{
    // TODO: observe the state of connections to the rooms. With structure
    // like this: {'room': is_connected}

    // To join string like ":lite5 JOIN #test_room_name" should be sent
    std::string send_str = ":" + nick + " JOIN " + room_name + "\r\n";
    SendOrDie(send_str);

    // after joining to the room start logging users' messages
    logger_.EnableLogging();
    logger_.Log("Successfully joined to " + room_name);
}

void IrcClient::Communicate()
{
    char recv_buf[kRecvBufLen] = { 0 };
    char iconv_buf[kIconvBufLen] = { 0 };
    int res = 0, ready = 0;

    struct pollfd fds[1];
    fds[0].fd = socket_;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    const unsigned int nfds = 1;
    const unsigned int timeout_in_msecs = 1000;

    unsigned int no_new_info_counter = 0;
    bool client_joined = false;

    LibiconvWrapper converter(config_.encoding(), "UTF-8");
    while(1)
    {
        memset(recv_buf, 0, sizeof(recv_buf));
        memset(iconv_buf, 0, sizeof(iconv_buf));
        ready = poll(fds, nfds, timeout_in_msecs);

        if (ready > 0)
        {
            no_new_info_counter = 0;

            // reduce receive length by 1 to have null-terminated string
            res = recv(socket_, recv_buf, sizeof(recv_buf)-1, kNoFlags);
            if (res == -1)
            {
                perror("recv failed!");
                logger_.Log("recv failed!");

                throw std::runtime_error("recv error");
            }
            else if (res == kRecvConnectionClosedByPeer)
            {
                logger_.Log("Peer closed the connection? Will try to reconnect");
                return;
            }

            converter.ConvertBuffer(recv_buf, kRecvBufLen, iconv_buf, kIconvBufLen);

            logger_.Log("received: \"" + std::string(iconv_buf) + "\"");

            std::vector<std::string> const messages = SplitOnLines(iconv_buf);
            for (const auto& msg : messages)
            {
                if (IsAutomaticallyHandledMsg(msg.c_str()))
                {
                    continue;
                }

                Message message(msg);
                LogPrettifiedMessage(message.GetStringForLogging());
            }
        }
        else if (ready == 0)
        {
            no_new_info_counter++;
            if (no_new_info_counter >= 3 && !client_joined)
            {
                JoinRoom(config_.nick(), config_.room());
                client_joined = true;
            }
        }
        else if (ready == -1)
        {
            perror("poll failed!");
            logger_.Log("poll failed!");

            throw std::runtime_error("poll error");
        }
    }
}

void IrcClient::SendPONG(const char* recv_buf)
{
    assert(recv_buf != NULL);
    if (recv_buf == NULL)
    {
        return;
    }
    logger_.Log("got PING: \"" + std::string(recv_buf) + "\"");
    const std::string kPongString = std::string("PONG");

    std::string received_str = std::string(recv_buf);
    size_t pos = received_str.find(":");

    std::string pong_msg = kPongString + " " + config_.nick() + " ";
    if (pos != std::string::npos)
    {
        // msg is smth like ":77E488E"
        std::string msg = received_str.substr(pos);
        pong_msg += msg;
        SendOrDie(pong_msg);
    }
    else
    {
        perror("SendPONG() error!");
        return;
    }
}

bool IrcClient::IsAutomaticallyHandledMsg(const char* recv_buf)
{
    if (IfPingRequest(recv_buf))
    {
        SendPONG(recv_buf);
        return true;
    }
    if (IfErrorCommand(recv_buf))
    {
        logger_.Log("Got unexpected 'exit' message from server. Will try to restart");
        throw std::runtime_error("Got unexpected 'exit' message from server");
    }
    return false;
}

void IrcClient::SendOrDie(const std::string& send_str)
{
    int res = 0;
    res = send(socket_, send_str.c_str(), send_str.length(), kNoFlags);
    if (res == -1)
    {
        perror("send failed!");

        throw std::runtime_error("send error");
    }

    logger_.Log("Successfully sent \"" + send_str + "\"");
}

void IrcClient::LogPrettifiedMessage(const std::string& message)
{
    assert(!message.empty());

    std::string log_message = message;
    size_t pos = message.rfind("\r\n");
    if (pos != std::string::npos)
    {
        log_message = message.substr(0, pos);
    }

    messages_logger_.Log(log_message);
}
