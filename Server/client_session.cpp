//
// Created by Namat on 11.08.2017.
//

#include "client_session.hpp"
#include <iostream>

client_session::client_session(ip::tcp::socket service) :
        socket_(std::move(service)),already_read_(0)
{
}

ip::tcp::socket& client_session::socket() {
    return socket_;
}

void client_session::start() {
    try {
        while(true) {
            read_request();
            process_request();
        }
    } catch ( boost::system::system_error& error) {
        std::cerr << "Error " << username_ << " : " << error.what() << std::endl;
        stop();
    }
}

void client_session::read_request() {
    if(socket_.available())
        already_read_ += socket_.read_some(asio::buffer(buffer_+already_read_,MAX_MSG - already_read_));
}

void client_session::process_request() {
    bool found_eof = std::find(buffer_, buffer_ + already_read_, DELIM)
                     < buffer_ + already_read_;
    if ( !found_eof) return;

    auto pos = std::find(buffer_, buffer_ + already_read_, DELIM) - buffer_;
    std::string msg(buffer_,pos);
    already_read_ -= pos + 1;

    if ( msg.find("login ") == 0) on_login(msg);
    else if (msg.find("-> exit") == 0) on_exit();
    else if ( msg.find("-> ") == 0) on_command(msg);
    else std::cerr << "Invalid msg " << username_ << " : " << msg << std::endl;
}

void client_session::stop() {
    boost::system::error_code error;
    socket_.close(error);
}

void client_session::on_login(const std::string& msg) {
    std::istringstream in(msg);
    in >> username_ >> username_;
    std::clog << username_ << " connected" << std::endl;
    write("login ok");
}

void client_session::on_command(const std::string& msg) {
    std::clog << username_ << " : " << msg << std::endl;
    write("command accepted");
}

void client_session::on_exit() {
    std::clog << username_ << " disconnect" << std::endl;
    stop();
}

void client_session::write(const std::string &msg) {
    socket_.write_some(asio::buffer(msg + DELIM));
}

