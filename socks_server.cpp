#include "socks_server.hpp"
using namespace std;

void sigchildHandler(int signo)
{
    pid_t child;
    while((child = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        d_trace("#child[%d] has sent sigchild", child);
    }
}

void server(int listenfd)
{
    int connfd;
    socklen_t client_len;
    struct sockaddr_in client_addr;
    char client_ip[INET_ADDRSTRLEN];
    vector<Firewall> firewalls;

    struct sigaction act_child;
    act_child.sa_handler = sigchildHandler;
    act_child.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &act_child, 0);

    while(true)
    {
        connfd = accept(listenfd, (struct sockaddr*) &client_addr, &client_len);

        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        d_server("New client from: %s:%d", client_ip, ntohs(client_addr.sin_port));

        pid_t pid = fork();
        if(pid < 0)
        {
            perror("fork error");
            close(connfd);
            continue;
        }

        if(pid == 0)
        {
            close(listenfd);
            SOCKSHandler handler(connfd, client_ip, ntohs(client_addr.sin_port));
            exit(EXIT_SUCCESS);
        }

        close(connfd);
    }
}

