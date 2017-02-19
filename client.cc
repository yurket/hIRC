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
#include <iostream>
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
    , logger_(Logger("irc_history.log"))
    , config_(std::move(xml_config))
{
    // disable logging for not to log  greeting messages from IRC server
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
    logger_.Log("[D] Trying to connect to server " + server_ip);
    std::cerr << "[D] Trying to connect to server " << server_ip << ":" << server_port << std::endl;

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
    std::string send_str;
    bool verbose=true;

    send_str = "PASS 12345\r\n";
    SendOrDie(send_str, verbose);

    send_str = "NICK " + nick + "\r\n";
    SendOrDie(send_str, verbose);

    send_str = "USER " + nick + " 0 * :" + real_name + "\r\n";
    SendOrDie(send_str, verbose);

    logger_.Log("Successfully registered with name " + nick);
}

void IrcClient::JoinRoom(const std::string& nick, const std::string& room_name)
{
    // TODO: observe the state of connections to the rooms. With structure
    // like this: {'room': is_connected}

    // To join string like ":lite5 JOIN #test_room_name" should be sent
    std::string send_str;
    bool verbose = true;

    send_str = ":" + nick + " JOIN " + room_name + "\r\n";
    SendOrDie(send_str, verbose);

    // after joining to the room start logging users' messages
    logger_.EnableLogging();
    logger_.Log("[+] Successfully joined to " + room_name);
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
                logger_.Log("[D] Peer closed the connection? Will try to reconnect");
                std::cerr << "Peer closed the connection? Will try to reconnect" << std::endl;
                return;
            }

            converter.ConvertBuffer(recv_buf, kRecvBufLen, iconv_buf, kIconvBufLen);

            std::cerr << "received: " << std::endl;
            std::cerr << iconv_buf << std::endl;

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
    std::cerr << "[D] got PING: " << std::endl << recv_buf << std::endl;
    const std::string kPongString = std::string("PONG");

    std::string received_str = std::string(recv_buf);
    size_t pos = received_str.find(":");

    std::string pong_msg = kPongString + " " + config_.nick() + " ";
    if (pos != std::string::npos)
    {
        // msg is smth like ":77E488E"
        std::string msg = received_str.substr(pos);
        pong_msg += msg;
        std::cerr << "[D] Sending PONG msg: " << pong_msg << std::endl;
        SendOrDie(pong_msg, true);
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

void IrcClient::SendOrDie(const std::string& send_str, bool verbose)
{
    int res = 0;
    res = send(socket_, send_str.c_str(), send_str.length(), kNoFlags);
    if (res == -1)
    {
        perror("send failed!");

        throw std::runtime_error("send error");
    }
    else if(verbose)
    {
        std::cerr << "SendOrDie(): '" << send_str << "' successfully sent" << std::endl;
    }
}

void IrcClient::LogPrettifiedMessage(const std::string& message)
{
    if (message.empty())
    {
        logger_.Log("[!] empty message");
    }
    if (message.find("PING :") != std::string::npos)
    {
        return;
    }

    std::string log_message = message;

    size_t pos = message.rfind("\r\n");
    if (pos != std::string::npos)
    {
        log_message = message.substr(0, pos);
    }

    logger_.Log(log_message);
}
