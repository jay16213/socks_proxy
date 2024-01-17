#include <arpa/inet.h>
#include <iostream>
#include <cstring>

#include "debug_logger.hpp"
#include "socks_server.hpp"
#include "util.hpp"
using namespace std;

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        cerr << "Usage: ./socks_server <port>" << endl;
        exit(EXIT_FAILURE);
    }

    int listenfd;

    if((listenfd = createTcpServer(atoi(argv[1]))) < 0)
    {
        cerr << "create tcp server error" << endl;
        exit(EXIT_FAILURE);
    }

    d_server("Server is listening on port %s", argv[1]);
    server(listenfd);

    return 0;
}
