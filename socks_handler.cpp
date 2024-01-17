#include "socks_handler.hpp"
using namespace std;

SOCKSHandler::SOCKSHandler(int fd, char *client_ip, int client_port)
                            : client_fd(fd),
                              client_ip(client_ip),
                              client_port(client_port)
{
    memset(&this->request, 0, sizeof(SOCKSRequest));
    this->handle();
}

SOCKSHandler::~SOCKSHandler()
{
    close(this->client_fd);
}

void SOCKSHandler::parseRequest(char *raw_request)
{
    struct addrinfo hints, *res;
    char ip[INET6_ADDRSTRLEN];

    memcpy(&this->request, raw_request, USERID_OFFSET);

    this->request.dest_port = ntohs(this->request.dest_port);
    strncpy(this->request.user_id, raw_request + USERID_OFFSET, strlen(raw_request + USERID_OFFSET));

    // 0.0.0.x with nonzero x
    regex expr("^0\\.0\\.0\\.\[1-9]\\d{0,2}$");
    inet_ntop(AF_INET, &(this->request.dest_ip.s_addr), ip, INET_ADDRSTRLEN);

    if(regex_match(string(ip), expr))
    {
		strncpy(this->request.domain_name, raw_request + USERID_OFFSET + strlen(this->request.user_id) + 1, DOMAIN_NAME_SIZE);
        d_trace("parse domain name: %s", this->request.domain_name);

        if(getaddrinfo(this->request.domain_name, NULL, &hints, &res) != 0)
        {
            perror("getaddrinfo error");
            return;
        }

        for(auto p = res; p != nullptr; p = p->ai_next)
        {
            if(p->ai_family == AF_INET)
            {
                this->request.dest_ip = ((struct sockaddr_in *)(p->ai_addr))->sin_addr;
                break;
            }
        }
	}

    d_trace("socks request");
    d_trace("version: %d", this->request.version);
    d_trace("command: %d", this->request.command);
    d_trace("dest port: %d", this->request.dest_port);
    inet_ntop(AF_INET, &(this->request.dest_ip.s_addr), ip, sizeof(ip));
    d_trace("dest ip: %s", ip);
}

void SOCKSHandler::handle()
{
    char raw_request[RECVBUF_SIZE];

    read(this->client_fd, raw_request, RECVBUF_SIZE);
    this->parseRequest(raw_request);

    // only accept SOCKS4 request
    if(this->request.version != 4)
    {
        this->reply(request_rejected_or_failed, nullptr, nullptr);
        return;
    }

    if(!this->passFirewall())
    {
        this->reply(request_rejected_or_failed, nullptr, nullptr);
        return;
    }

    switch(this->request.command)
    {
    case CONNECT_MODE:
        this->connectMode();
        d_server("END OF CONNECT MODE");
        break;
    case BIND_MODE:
        this->bindMode();
        d_server("END OF BIND MODE");
        break;
    default:
        this->reply(request_rejected_or_failed, nullptr, nullptr);
        break;
    }
}

void SOCKSHandler::reply(ResultCode result_code, int *port, in_addr_t *ip, bool verbose)
{
    char res[8], dest_ip[INET6_ADDRSTRLEN];

    memset(res, 0, 8);
    inet_ntop(AF_INET, &(this->request.dest_ip.s_addr), dest_ip, INET6_ADDRSTRLEN);

    res[1] = result_code;
    if(port != nullptr)
    {
        *port = htons(*port);
        memcpy(res + 2, port, 2);
    }
    if(ip != nullptr)
        memcpy(res + 4, ip, 4);

    if(verbose)
    {
        fprintf(stdout, "<S_IP>: %s\n", this->client_ip.c_str());
        fprintf(stdout, "<S_PORT>: %d\n", this->client_port);
        fprintf(stdout, "<D_IP>: %s\n", dest_ip);
        fprintf(stdout, "<D_PORT>: %d\n", this->request.dest_port);
        fprintf(stdout, "<Command>: %s\n", this->request.command == CONNECT_MODE ? "CONNECT" : "BIND");
        fprintf(stdout, "<Reply>: %s\n", result_code == request_granted ? "Accept" : "Reject");
    }

    write(this->client_fd, res, 8);
}

void SOCKSHandler::connectMode()
{
    int dest_fd;

    if((dest_fd = this->connectToDest()) == -1)
    {
        this->reply(request_rejected_or_failed, nullptr, nullptr);
        return;
    }

    this->reply(request_granted, nullptr, nullptr);
    this->startSession(dest_fd);
}

void SOCKSHandler::bindMode()
{
    int listenfd, dest_fd, port = 0;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if((listenfd = createTcpServer(port)) < 0)
    {
        cerr << "create tcp server error" << endl;
        return;
    }

    if(getsockname(listenfd, (struct sockaddr*) &server_addr, &addrlen) < 0)
    {
        perror("getsockname error");
        return;
    }
    port = ntohs(server_addr.sin_port);
    d_server("SOCKS BIND mode Server is listening on port %d", port);

    this->reply(request_granted, &port, nullptr, false);

    if((dest_fd = accept(listenfd, (struct sockaddr*) &server_addr, &addrlen)) < 0)
    {
        perror("accept error");
        close(listenfd);
        return;
    }

    this->reply(request_granted, &port, nullptr, true);
    this->startSession(dest_fd);
}

int SOCKSHandler::connectToDest()
{
    int dest_fd;
    struct sockaddr_in dest_addr;

    dest_fd = socket(AF_INET, SOCK_STREAM, 0);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr = this->request.dest_ip;
    dest_addr.sin_port = htons(this->request.dest_port);

    if(connect(dest_fd, (struct sockaddr*) &dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("Connect error");
        close(this->client_fd);
        close(dest_fd);
        return -1;
    }

    return dest_fd;
}

void SOCKSHandler::startSession(int dest_fd)
{
    fd_set read_set, all_set;
    int maxfd;
    ssize_t nbytes = 0;
    unsigned char recvbuf[RECVBUF_SIZE] = {0};

    maxfd = (this->client_fd > dest_fd ? this->client_fd : dest_fd) + 1;
    FD_ZERO(&all_set);
    FD_SET(this->client_fd, &all_set);
    FD_SET(dest_fd, &all_set);

    while(__FDS_BITS(&all_set))
    {
        memcpy(&read_set, &all_set, sizeof(fd_set));

        if(select(maxfd, &read_set, nullptr, nullptr, nullptr) < 0)
        {
            perror("select error");
            break;
        }

        if(FD_ISSET(this->client_fd, &read_set))
        {
            nbytes = read(this->client_fd, recvbuf, RECVBUF_SIZE);
            if(nbytes < 0)
            {
                perror("read error");
                break;
            }

            if(nbytes == 0) // end of session
                break;

            write(dest_fd, recvbuf, nbytes);
        }

        if(FD_ISSET(dest_fd, &read_set))
        {
            nbytes = read(dest_fd, recvbuf, RECVBUF_SIZE);
            if(nbytes < 0)
            {
                perror("read error");
                break;
            }

            if(nbytes == 0) // end of session
                break;

            write(this->client_fd, recvbuf, nbytes);
        }
    }

    close(dest_fd);
}

bool SOCKSHandler::passFirewall()
{
    char tmp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(this->request.dest_ip.s_addr), tmp, INET_ADDRSTRLEN);
    string dest_ip(tmp);

    loadFirewallConfig(this->firewalls);

    regex dot_expr("\\.");
    regex star_expr("\\*");
    for(auto f : this->firewalls)
    {
        string ip_str(f.ip);
        ip_str = regex_replace(ip_str, dot_expr, "\\.");
        ip_str = regex_replace(ip_str, star_expr, "\\d{1,3}");

        if(this->request.command == f.SOCKS_mode)
        {
            if(regex_match(dest_ip, regex(ip_str)))
            {
                if(f.rule == PERMIT)
                {
                    d_server("pass %d %s", f.SOCKS_mode, f.ip.c_str());
                    return true;
                }
            }
        }
    }

    return false;
}
