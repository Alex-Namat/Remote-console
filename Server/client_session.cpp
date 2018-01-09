//
// Created by Namat on 11.08.2017.
//

#include "client_session.hpp"
#include <iostream>

client_session::client_session(ip::tcp::socket socket) :
        socket_(std::move(socket))
{
    last_ping_ = chrono::steady_clock::now();
}

ip::tcp::socket& client_session::socket() {
    return socket_;
}

void client_session::start() noexcept {
    try {
        process_.run();
        while(!timed_out() && process_.is_active()) {
            read_request();
            process_request();
        }
        stop();
    } catch (const std::exception& error) {
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
    if ( !found_eof) return;    // сообщение не полное

    last_ping_ = chrono::steady_clock::now();

    auto pos = std::find(buffer_, buffer_ + already_read_, DELIM) - buffer_;
    std::string msg(buffer_,pos);
    already_read_ -= pos + 1;

    std::clog << username_ << ": " << msg << std::endl;

    std::string s_login("#login "), s_ping("#ping");
    //ищем спец.команды
    if ( msg.compare(0,s_login.size(),s_login) == 0) on_login(msg);
    else if(msg.compare(0,s_ping.size(),s_ping) == 0) on_ping();
    else on_command(msg);
}

void client_session::stop() noexcept {
    std::clog << username_ << " disconnect" << std::endl;
    if (process_.is_active()) process_.stop();
    boost::system::error_code error;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error);
    if(error){
        std::cerr << "Error " << username_ << " : " << error << std::endl;
    }
    socket_.close(error);
    if(error){
        std::cerr << "Error " << username_ << " : " << error << std::endl;
    }
}

void client_session::on_login(const std::string& msg) {
    std::istringstream in(msg);
    in >> username_ >> username_;
    std::clog << username_ << " connected" << std::endl;
    write(process_.read(buffer_,MAX_MSG));
}

void client_session::on_command(const std::string& msg) {
    process_.write(msg);
    if (!process_.is_active()){
        on_exit();
        return;
    }
    std::string str = process_.read(buffer_,MAX_MSG);
    if(process_.buffer_overflow()){
        std::string tmp = "#buffer_overflow\n";
        write(tmp + std::string(str, 0, MAX_MSG - tmp.size()));
        process_.stop();
        return;
    }
    write(str);
}

void client_session::on_exit(){
    write("#disconnect");
}

void client_session::write(const std::string &msg){
    socket_.write_some(asio::buffer(msg + DELIM));
}

void client_session::on_ping() const noexcept {
}

bool client_session::timed_out() const noexcept {
    return chrono::steady_clock::now() - last_ping_ > chrono::minutes(1);
}

