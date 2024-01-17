#ifndef __SOCKS_REQUESR_HPP__
#define __SOCKS_REQUESR_HPP__

#include <arpa/inet.h>

#define USERID_OFFSET      8
#define USER_ID_SIZE     256
#define DOMAIN_NAME_SIZE 512

#define CONNECT_MODE 1
#define BIND_MODE    2

enum ResultCode{
    request_granted = 90,
    request_rejected_or_failed
};

typedef struct _SOCKSRequest{
    uint8_t version;
    uint8_t command;
    uint16_t dest_port;
    struct in_addr dest_ip;
    char user_id[USER_ID_SIZE];
    char domain_name[DOMAIN_NAME_SIZE];
} SOCKSRequest;

#endif
