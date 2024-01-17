#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstring>
#include <cstdio>

#define LISTENQ 128

int createTcpServer(int port);

#endif
