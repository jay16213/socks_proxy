#include "util.hpp"
using namespace std;

int createTcpServer(int port)
{
    int listenfd;
    const int enable = 1;
    struct sockaddr_in server_addr;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket error");
        return -1;
    }

    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
    {
        perror("setsockopt error");
        return -1;
    }

    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) == -1)
    {
        perror("setsockopt error");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if(bind(listenfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind error");
        return -1;
    }

    if(listen(listenfd, LISTENQ) == -1)
    {
        perror("listen error");
        return -1;
    }

    return listenfd;
}
