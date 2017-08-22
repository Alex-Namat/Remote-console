#include <iostream>
#include "sync_tcp_client.hpp"

int main() {
    sync_tcp_client client(ip::address::from_string("127.0.0.1"),8001);
    std::string str;
    std::cin >> str;
    client.run_client(str);
}