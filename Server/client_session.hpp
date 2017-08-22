//
// Created by Namat on 11.08.2017.
//

#ifndef SERVER_CLIENT_SESSION_HPP
#define SERVER_CLIENT_SESSION_HPP

#include <memory>
#include <chrono>

#include <boost/asio.hpp>

static constexpr int MAX_MSG = 1024;
static constexpr char  DELIM = '\0';

namespace asio = boost::asio;
namespace ip = asio::ip;
namespace chrono = std::chrono;

class client_session : std::enable_shared_from_this<client_session>{
public:
    explicit client_session(ip::tcp::socket service);
    ip::tcp::socket& socket();
    void start();

private:
    void read_request();
    void process_request();
    void stop();

    void on_login(const std::string& msg);
    void on_command(const std::string& msg);
    void on_exit();

    void on_ping();
    bool timed_out();

    void write(const std::string& msg);

private:
    ip::tcp::socket socket_;
    std::string username_;
    size_t already_read_;
    char buffer_[MAX_MSG];

    chrono::time_point<chrono::steady_clock> last_ping_;
};


#endif //SERVER_CLIENT_SESSION_HPP
