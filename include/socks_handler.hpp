#ifndef __SOCKS_HANDLER_HPP__
#define __SOCKS_HANDLER_HPP__

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <cstdio>
#include <regex>

#include "socks_request.hpp"
#include "debug_logger.hpp"
#include "firewall.hpp"
#include "util.hpp"

#define RECVBUF_SIZE 65536

class SOCKSHandler
{
private:
    int client_fd;
    std::string client_ip;
    int client_port;
    SOCKSRequest request;
    std::vector<Firewall> firewalls;

    void parseRequest(char *raw_request);
    void reply(ResultCode result_code, int *port, in_addr_t *ip, bool verbose = true);
    int connectToDest();
    void connectMode();
    void bindMode();
    void startSession(int dest_fd);
    bool passFirewall();
public:
    SOCKSHandler(int fd, char *client_ip, int client_port);
    ~SOCKSHandler();

    void handle();
};

#endif
