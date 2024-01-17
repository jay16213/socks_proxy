#include "firewall.hpp"
using namespace std;

void loadFirewallConfig(vector<Firewall> &firewalls)
{
    firewalls.clear();
    ifstream config(FIREWALL_CONFIG);

    if(config.fail())
    {
        d_trace("Can not load %s", FIREWALL_CONFIG);
        return;
    }

    string line, token;
    while(getline(config, line))
    {
        stringstream ss(line);
        Firewall f;

        getline(ss, token, ' ');
        f.rule = token;

        getline(ss, token, ' ');
        if(token == "c")
            f.SOCKS_mode = CONNECT_MODE;
        if(token == "b")
            f.SOCKS_mode = BIND_MODE;

        getline(ss, token, ' ');
        f.ip = token;

        firewalls.push_back(f);
    }
}
