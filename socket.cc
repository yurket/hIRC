#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;


const short RECV_LEN = 512;
const short SEND_LEN = 256;
const char *irc_bynets_org = "86.57.151.5";
const char DELIMITER[2] = {'\r', '\n' };
const short NO_FLAGS = 0;

int form_message(char *send_buf)
{
    memset(send_buf, 0, SEND_LEN);

    cout << "enter command: " << endl;
    cin.getline(send_buf, SEND_LEN);

    if (strncmp(send_buf, "pong", 4) == 0)
    {
        memset(send_buf, 0, SEND_LEN);
        strcat(send_buf, "PONG ");
        strcat(send_buf, irc_bynets_org);
    }
    strcat(send_buf, DELIMITER);

    return 0;
}

int main()
{
    int sock = 0;
    int res = 0;
    char recv_buf[RECV_LEN];
    char send_buf[SEND_LEN];
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, NO_FLAGS);
    if (sock == -1)
    {
        perror("Can't create socket!");
        _exit(1);
    }

    memset(&addr, 0, sizeof(struct sockaddr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6667);
    res = inet_aton(irc_bynets_org, &addr.sin_addr);
    if (res != 1)
    {
        perror("inet_pton error!");
        _exit(1);
    }

    res = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        perror("connect error!");
        close(sock);
        _exit(1);
    }

    while(1){
        memset(recv_buf, 0, sizeof(recv_buf));
        res = recv(sock, recv_buf, RECV_LEN, NO_FLAGS);
        if (res == -1)
        {
            perror("recv failed!");

            close(sock);
            _exit(1);
        }
        cout << "received: " << endl;
        cout << recv_buf << endl;

        form_message(send_buf);

        short send_size = strlen(send_buf);
        res = send(sock, send_buf, send_size, NO_FLAGS);
        if (res == -1)
        {
            perror("send failed!");

            close(sock);
            _exit(1);
        }
        else
        {
            clog << res << " bytes sent... " << endl;
            clog << " (echoed) " << send_buf << endl;
        }

    }
    return 0;
}
