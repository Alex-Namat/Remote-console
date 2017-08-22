//
// Created by Namat on 31.07.2017.
//

#include "sync_tcp_server.hpp"

#include <thread>

namespace ip = asio::ip;

using client_ptr = std::shared_ptr<client_session>;

void sync_tcp_server::start() {
    ip::tcp::acceptor acceptor (service_, ip::tcp::endpoint(ip::tcp::v4(),PORT));
    while(true){
        client_ptr new_client = std::make_shared<client_session>(ip::tcp::socket(service_));
        acceptor.accept(new_client->socket());
        std::thread(std::bind(&client_session::start,new_client)).detach();
    }
}
