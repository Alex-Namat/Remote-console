//
// Created by Namat on 11.08.2017.
//

#include "sync_tcp_client.hpp"

#include <iostream>
#include <thread>

sync_tcp_client::sync_tcp_client(const asio::ip::address &address, const int &port) :
        ep(address,port),sock_(service),started_(true){

}

void sync_tcp_client::run_client(const std::string &client_name) {
    username_ = client_name;
    try {
        connect();
        loop();
    } catch(boost::system::system_error & err) {
        std::cout << "client terminated " << username_
                  << ": " << err.what() << std::endl;
    }
}

void sync_tcp_client::connect() {
    sock_.connect(ep);
}

void sync_tcp_client::loop() {
    write("login " + username_);
    read_answer();
    while ( started_) {
        write_request();
        read_answer();
    }
}

void sync_tcp_client::write_request() {
    std::string msg;
    std::cout << "-> ";
    std::cin >> msg;
    write("-> "+ msg);
}

void sync_tcp_client::read_answer() {
    already_read_ = 0;
    using namespace std::placeholders;
    read(sock_, asio::buffer(buff_),
         std::bind(&sync_tcp_client::read_complete, this, _1, _2));
    process_msg();
}

void sync_tcp_client::process_msg() {
    std::string msg(buff_, already_read_);
    if ( msg.find("login ok") == 0) on_login();
    else std::cout << "$ " << msg << std::endl;
}

void sync_tcp_client::on_login() {
    std::cout << "Connected." << std::endl;
}

void sync_tcp_client::write(const std::string &msg) {
    sock_.write_some(asio::buffer(msg + DELIM));
}

size_t sync_tcp_client::read_complete(const boost::system::error_code &err, size_t bytes) {
    if ( err) return 0;
    already_read_ = bytes;
    bool found = std::find(buff_, buff_ + bytes, '\0') < buff_ + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}
