#ifndef __SOCKS_SERVER_HPP__
#define __SOCKS_SERVER_HPP__

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstdio>

#include "socks_handler.hpp"
#include "debug_logger.hpp"

void server(int listenfd);

#endif
