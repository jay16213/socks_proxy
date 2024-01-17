#ifndef __FIREWALL_HPP__
#define __FIREWALL_HPP__

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "socks_request.hpp"
#include "debug_logger.hpp"

#define FIREWALL_CONFIG "socks.conf"

#define PERMIT "permit"

typedef struct _Firewall{
    std::string rule;
    uint8_t SOCKS_mode;
    std::string ip;
} Firewall;

void loadFirewallConfig(std::vector<Firewall> &firewalls);

#endif
