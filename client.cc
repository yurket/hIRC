#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "libiconv_wrapper.h"

namespace
{

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
        result.push_back(text.substr(last_pos, (endline_pos-last_pos)));
        last_pos = endline_pos;
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

} // namespace



/************************** PUBLIC **************************/

IrcClient::IrcClient(const std::string &config_filename)
    : logger_(Logger("test.log"))
    , config_(XmlConfig(config_filename))
{
    // disable logging for not to log  greeting messages from IRC server
    logger_.DisableLogging();
}


void IrcClient::Connect(const std::string &server_ip, const unsigned int server_port)
{
    int res = 0;
    struct sockaddr_in addr;

    socket_ = socket(AF_INET, SOCK_STREAM, kNoFlags);
    if (socket_ == -1)
    {
        perror("Can't create socket");
        _exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    res = inet_aton(server_ip.c_str(), &addr.sin_addr);
    if (res != 1)
    {
        perror("inet_aton error");
        close(socket_);
        _exit(1);
    }

    res = connect(socket_, (const struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        perror("connect error");
        close(socket_);
        _exit(1);
    }
    logger_.Log("Successfully connected");
}


// The quotation from RFC 2812:
/*
3.1 Connection Registration

   The commands described here are used to register a connection with an
   IRC server as a user as well as to correctly disconnect.

   A "PASS" command is not required for a client connection to be
   registered, but it MUST precede the latter of the NICK/USER
   combination (for a user connection) or the SERVICE command (for a
   service connection). The RECOMMENDED order for a client to register
   is as follows:

                           1. Pass message
           2. Nick message                 2. Service message
           3. User message

   Upon success, the client will receive an RPL_WELCOME (for users) or
   RPL_YOURESERVICE (for services) message indicating that the
   connection is now registered and known the to the entire IRC network.
   The reply message MUST contain the full client identifier upon which
   it was registered.
 */
void IrcClient::Register(const std::string &nick, const std::string &real_name)
{
    std::string send_str;
    bool verbose=true;

    // 1. Pass message
    send_str = "PASS 12345\r\n";
    SendOrDie(send_str, verbose);

    // 2. Nick message
    send_str = "NICK " + nick + "\r\n";
    SendOrDie(send_str, verbose);

    // 3. User message
    send_str = "USER " + nick + " 0 * :" + real_name + "\r\n";
    SendOrDie(send_str, verbose);

    logger_.Log("Successfully registered with name " + nick);
}

void IrcClient::JoinRoom(const std::string &nick, const std::string &room_name)
{
    // TODO: observe the state of connections to the rooms. With structure
    // like this: {'room': is_connected}

    // To join string like this: ":lite5 JOIN #test_room_name" should be sent
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
    char send_buf[kSendBufLen] = { 0 };
    int res = 0, ready = 0;
    unsigned int send_size = 0;

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

                close(socket_);
                _exit(1);
            }

            converter.ConvertBuffer(recv_buf, kRecvBufLen, iconv_buf, kIconvBufLen);

            std::cerr << "received: " << std::endl;
            std::cerr << iconv_buf << std::endl;

            // process each line separately
            std::vector<std::string> const messages = SplitOnLines(iconv_buf);
            for (auto& msg : messages)
            {
                if (AutomaticallyHandledMsg(msg.c_str()))
                {
                    continue;
                }
                LogPrettifiedMessage(msg.c_str());
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

            close(socket_);
            _exit(1);
        }

        /* Debug code */
        // cin.getline(send_buf, kSendBufLen);
        // if (!strcmp(send_buf, "join"))
        // {
        //     JoinRoom(config_.nick(), config_.room());
        //     memset(send_buf, 0, kSendBufLen);
        // }
        // else{
        //     strcat(send_buf, "\r\n");
        //     SendOrDie(send_buf, true);
        //     memset(send_buf, 0, kSendBufLen);
        // }
    }
}

/************************** PRIVATE  **************************/

void IrcClient::SendPONG(const char *recv_buf)
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

    // TODO: sprintf?
    std::string pong_msg = kPongString + " " + config_.nick() + " ";
    if (pos != std::string::npos)
    {
        // msg is smth like ":77E488E" for bynets server
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

/*
  returns *TRUE*, if message was automatically handled and
  *FALSE* otherwise.
 */
bool IrcClient::AutomaticallyHandledMsg(const char *recv_buf)
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
        // JoinRoom(config_.nick(), config_.room());
        // return true;
    }
    return false;
}

/*
  Send *buf* of length *len* using opened class member *socket_*.
 */
void IrcClient::SendOrDie(const std::string &send_str, bool verbose)
{
    int res = 0;
    res = send(socket_, send_str.c_str(), send_str.length(), kNoFlags);
    if (res == -1)
    {
        perror("send failed!");

        close(socket_);
        _exit(1);
    }
    else if(verbose)
    {
        std::cerr << "SendOrDie(): '" << send_str << "' successfully sent" << std::endl;
    }
}

void IrcClient::LogPrettifiedMessage(const std::string &message)
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

    // remove trailing \r\n
    size_t pos = message.rfind("\r\n");
    if (pos != std::string::npos)
    {
        log_message = message.substr(0, pos);
    }

    logger_.Log(log_message);
}


int main()
{
    XmlConfig config("bynets.xml");
    config.print_config();

    while(true)
    {
        IrcClient client("bynets.xml");
        std::cerr << std::endl << std::endl;

        client.Connect(config.server_ip(), config.server_port());
        client.Register(config.nick(), config.real_name());
        try
        {
            client.Communicate();
        }
        catch (const std::exception& e)
        {
            continue;
        }
    }
    return 0;
}
