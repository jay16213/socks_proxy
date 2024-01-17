#ifndef __DEBUG_LOGGER_HPP__
#define __DEBUG_LOGGER_HPP__

#ifdef DEBUG
#include <cstdio>
#define d_trace(fmt, ...) {fprintf(stderr, "\e[33m[DEBUG] " fmt "\e[0m\n", ## __VA_ARGS__); fflush(stderr);}
#define d_server(fmt, ...) {fprintf(stdout, "\e[32m[SERVER] " fmt "\e[0m\n", ## __VA_ARGS__); fflush(stderr);}
#else
#define d_trace(fmt, ...) ;
#define d_server(fmt, ...) ;
#endif

#endif
