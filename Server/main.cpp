#include <iostream>
#include "sync_tcp_server.hpp"

int main() {
    sync_tcp_server server;
    server.start();
    return 0;
}