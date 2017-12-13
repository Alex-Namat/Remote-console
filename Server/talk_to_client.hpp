//
// Created by Namat on 31.07.2017.
//

#ifndef SERVER_TALK_TO_CLIENT_HPP
#define SERVER_TALK_TO_CLIENT_HPP

#include <memory>
#include <string>

#include <boost/asio.hpp>
#include "sync_tcp_server.hpp"

class sync_tcp_server;

class talk_to_client : std::enable_shared_from_this<talk_to_client>{
    sync_tcp_server& server;
    boost::asio::ip::tcp::socket socket;
    static const int max_msg = 1024;
    int already_read;
    char buff[max_msg];
    bool started;
    std::string username;
    bool clients_changed;
    boost::posix_time::ptime last_ping;

    void read_request();
    void process_request();
    void on_login(const std::string& msg);
    void on_ping();
    void on_clients();
    void write(const std::string& msg);

public:
    talk_to_client(sync_tcp_server& server);
    std::string login() const;
    void answer_to_client();
    void set_clients_changed();
    boost::asio::ip::tcp::socket& sock();
    bool timed_out() const;
    void stop();
};


#endif //SERVER_TALK_TO_CLIENT_HPP
