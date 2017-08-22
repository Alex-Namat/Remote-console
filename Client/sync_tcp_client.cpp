//
// Created by Namat on 11.08.2017.
//

#include "sync_tcp_client.hpp"

#include <iostream>
#include <thread>

sync_tcp_client::sync_tcp_client(const asio::ip::address &address, const int &port) :
        ep(address,port),socket_(service_){

}

void sync_tcp_client::run_client(const std::string &client_name) {
    username_ = client_name;
    try {
        connect();
        loop();
    } catch(boost::system::system_error & error) {
        if (error.code() == asio::error::eof)
            return;
        std::cerr << error.what() << std::endl;
    }
}

void sync_tcp_client::connect() {
    socket_.connect(ep);
}

void sync_tcp_client::loop() {
    write("login " + username_);
    read_answer();
    std::thread(std::bind(&sync_tcp_client::pinged,this)).detach();
    while(true) {
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
    read(socket_, asio::buffer(buffer_),
         std::bind(&sync_tcp_client::read_complete, this, _1, _2));
    process_msg();
}

void sync_tcp_client::process_msg() {
    std::string msg(buffer_, already_read_);
    if ( msg.find("login ok") == 0) on_login();
    else std::cout << msg << std::endl;
}

void sync_tcp_client::on_login() {
    std::cout << "Connected" << std::endl;
}

void sync_tcp_client::write(const std::string &msg) {
    socket_.write_some(asio::buffer(msg + DELIM));
}

size_t sync_tcp_client::read_complete(const boost::system::error_code &err, size_t bytes) {
    if ( err) return 0;
    already_read_ = bytes;
    bool found = std::find(buffer_, buffer_ + bytes, '\0') < buffer_ + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

void sync_tcp_client::pinged() {
    while(true) {
        write("ping");
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}
