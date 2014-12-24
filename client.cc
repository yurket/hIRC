#include "client.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>


void IrcClient::Connect(const std::string server_ip, const unsigned int server_port)
{
    int res = 0;
    struct sockaddr_in addr;

    socket_ = socket(AF_INET, SOCK_STREAM, kNoFlags);
    if (socket_ == -1)
    {
        perror("Can't create socket!");
        _exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    res = inet_aton(server_ip.c_str(), &addr.sin_addr);
    if (res != 1)
    {
        perror("inet_pton error!");
        close(socket_);
        _exit(1);
    }

    res = connect(socket_, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        perror("connect error!");
        close(socket_);
        _exit(1);
    }
}

bool IfPingRequest(const char *recv_buf)
{
    const std::string kPingString = std::string("\nPING");

    std::string received_str = std::string(recv_buf);
    if (received_str.find(kPingString) == std::string::npos)
        return false;
    else
        return true;
}

void IrcClient::FormPongResponse(const char *recv_buf, char *send_buf)
{

}



void IrcClient::FormMessage(const char *recv_buf, char *send_buf)
{
    const char kDelimiter[2] = {'\r', '\n'};

    memset(send_buf, 0, kSendBufLen);

    if (IfPingRequest(recv_buf))
    {
        cout << "PING came -> answering PONG" << endl;
        FormPongResponse(recv_buf, send_buf);
    }
    else
    {
        cout << "enter command: " << endl;
        cin.getline(send_buf, kSendBufLen);
        strncat(send_buf, kDelimiter, 2);
    }
}

/*
  Send *buf* of length *len* using opened class member *socket_*.
 */
void IrcClient::SendOrDie(const std::string &send_str, bool verbose)
{
    unsigned int res = 0;
    res = send(socket_, send_str.c_str(), send_str.length(), kNoFlags);
    if (res == -1)
    {
        perror("send failed!");

        close(socket_);
        _exit(1);
    }
    else if(verbose)
    {
        cout << send_str << " successfully sent" << std::endl;
    }
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
void IrcClient::Register(const std::string nick, const std::string real_name)
{
    std::string send_str;
    bool verbose=true;

    // 1. Pass message
    send_str.clear();
    send_str.append("PASS 12345\n");
    SendOrDie(send_str, verbose);

    // 2. Nick message
    send_str.clear();
    send_str.append("NICK ");
    send_str.append(nick);
    send_str.append("\n");
    SendOrDie(send_str, verbose);

    // 3. User message
    send_str.clear();
    send_str.append("USER ");
    send_str.append(nick);
    send_str.append(" 0 * :");
    send_str.append(real_name);
    send_str.append("\n");
    SendOrDie(send_str, verbose);
}

void IrcClient::Communicate()
{
    char recv_buf[kRecvBufLen];
    char send_buf[kSendBufLen];
    int res = 0, poll_res = 0;
    unsigned int send_size = 0;
    struct pollfd fds[1];
    fds[0].fd = socket_;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    // timeout in msecs
    const unsigned int timeout = 1000;

    while(1){
        poll_res = poll(fds, 1, timeout);
        if (poll_res > 0){
            memset(recv_buf, 0, sizeof(recv_buf));
            // reduce receive length by 1 to have null-terminated string
            res = recv(socket_, recv_buf, sizeof(recv_buf)-1, kNoFlags);
            if (res == -1)
            {
                perror("recv failed!");

                close(socket_);
                _exit(1);
            }
            cout << "received: " << endl;
            cout << recv_buf << endl;
        }
        else if (poll_res == -1)
        {
            perror("poll failed!");

            close(socket_);
            _exit(1);
        }

        FormMessage(recv_buf, send_buf);

        send_size = strlen(send_buf);
        res = send(socket_, send_buf, send_size, kNoFlags);
        if (res == -1)
        {
            perror("send failed!");

            close(socket_);
            _exit(1);
        }
        else
        {
            clog << res << " bytes sent... " << endl;
            clog << " (echoed) " << send_buf << endl;
        }
    }

}

int main()
{
    IrcClient client = IrcClient();
    XmlConfig config = XmlConfig("freenode.xml");
    config.print_config();
    cout << std::endl << std::endl;

    client.Connect(config.server_ip, config.server_port);
    client.Register(config.nick, config.real_name);
    client.Communicate();
    return 0;
}
