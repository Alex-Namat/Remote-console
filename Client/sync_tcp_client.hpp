//
// Created by Namat on 11.08.2017.
//

#ifndef CLIENT_SYNC_TCP_CLIENT_HPP
#define CLIENT_SYNC_TCP_CLIENT_HPP

#include <thread>
#include <boost/asio.hpp>

namespace asio = boost::asio;
namespace ip = asio::ip;

static constexpr int MAX_MSG = 1024*10;
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

    void write(const std::string & msg);
    size_t read_complete(const boost::system::error_code & err, size_t bytes);

    void pinged();

    asio::io_service service_;
    ip::tcp::endpoint ep;
    ip::tcp::socket socket_;
    
    size_t already_read_ {0};
    char buffer_[MAX_MSG];
    
    std::string username_;
    mutable std::atomic<bool> connected {true};
};


#endif //CLIENT_SYNC_TCP_CLIENT_HPP
