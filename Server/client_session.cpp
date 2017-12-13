//
// Created by Namat on 11.08.2017.
//

#include "client_session.hpp"
#include <iostream>

client_session::client_session(ip::tcp::socket service) :
        socket_(std::move(service)),already_read_(0),
        out(socket_.get_io_service()),child_("cmd",process::std_out>out,process::std_in<in,socket_.get_io_service())
{
    last_ping_ = chrono::steady_clock::now();
}

ip::tcp::socket& client_session::socket() {
    return socket_;
}

void client_session::start() {
    try {
        while(!timed_out() && socket_.is_open()) {
            read_request();
            process_request();
        }
        stop();
        std::clog << "Stopping " << username_ << std::endl;
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

    last_ping_ = chrono::steady_clock::now();

    auto pos = std::find(buffer_, buffer_ + already_read_, DELIM) - buffer_;
    std::string msg(buffer_,pos);
    already_read_ -= pos + 1;

    if ( msg.find("login ") == 0) on_login(msg);
    else if(msg.find("ping") == 0) on_ping();
    else if (msg.find("-> /q") == 0) on_exit();
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
    asio::async_read(out, asio::buffer(buffer_), [](const boost::system::error_code &, std::size_t) {});
    write(std::string(buffer_,MAX_MSG));
}

void client_session::on_command(const std::string& msg) {
    std::istringstream ss(msg);
    std::string str;
    ss >> str >> str;
    in << str << std::endl;
    asio::async_read(out, asio::buffer(buffer_), [](const boost::system::error_code &, std::size_t) {});
    write(std::string(buffer_,MAX_MSG));
}

void client_session::on_exit() {
    std::clog << username_ << " disconnect" << std::endl;
    child_.terminate();
    stop();
}

void client_session::write(const std::string &msg) {
    socket_.write_some(asio::buffer(msg + DELIM));
}

void client_session::on_ping() {
    std::clog << username_ << " pinged" << std::endl;
}

bool client_session::timed_out() {
    auto tmp = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - last_ping_);
    if(tmp.count() >= 60) std::cout << username_ << " " << tmp.count() << std::endl;
    return chrono::steady_clock::now() - last_ping_ > chrono::minutes(1);
}

