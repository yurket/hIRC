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

void IrcClient::FormMessage(const char *recv_buf, char *send_buf)
{
    const char kDelimiter[2] = {'\r', '\n'};

    memset(send_buf, 0, kSendBufLen);

    cout << "enter command: " << endl;
    cin.getline(send_buf, kSendBufLen);
    strncat(send_buf, kDelimiter, 2);
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
    cout << "test: " << 0x001 << endl;
    while(1){
        poll_res = poll(fds, 1, timeout);
        cout << "poll returned: " << poll_res << endl;
        if (poll_res > 0){
            cout << "revents: " << fds[0].revents << endl;
            memset(recv_buf, 0, sizeof(recv_buf));
            res = recv(socket_, recv_buf, sizeof(recv_buf), kNoFlags);
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

    client.Connect(config.server_ip, config.server_port);
    client.Communicate();
    return 0;
}
