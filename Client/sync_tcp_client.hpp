//
// Created by Namat on 11.08.2017.
//

#ifndef CLIENT_SYNC_TCP_CLIENT_HPP
#define CLIENT_SYNC_TCP_CLIENT_HPP


#include <boost/asio.hpp>

namespace asio = boost::asio;
namespace ip = asio::ip;

static constexpr int MAX_MSG = 1024;
static constexpr char DELIM = '\0';

class sync_tcp_client {
public:
    sync_tcp_client(const ip::address& address, const int& port);
    void run_client(const std::string& client_name);
private:

    void connect();
    void loop();
    void write_request();
    void read_answer();
    void process_msg();
    void on_login();
    void write(const std::string & msg);
    size_t read_complete(const boost::system::error_code & err, size_t bytes);

    asio::io_service service;
    ip::tcp::endpoint ep;
    ip::tcp::socket sock_;
    int already_read_;
    char buff_[MAX_MSG];
    bool started_;
    std::string username_;
};


#endif //CLIENT_SYNC_TCP_CLIENT_HPP
