//
// Created by Namat on 11.08.2017.
//

#ifndef SERVER_CLIENT_SESSION_HPP
#define SERVER_CLIENT_SESSION_HPP

#include <chrono>
#include <boost/asio.hpp>
#include "win_process.hpp"

static constexpr int MAX_MSG = 1024*10;
static constexpr char  DELIM = '\0';

namespace asio = boost::asio;
namespace ip = asio::ip;
namespace chrono = std::chrono;

class client_session : std::enable_shared_from_this<client_session>, boost::noncopyable{
public:
    explicit client_session(ip::tcp::socket socket);
    ip::tcp::socket& socket();
    void start() noexcept;

private:
    void read_request();
    void process_request();
    void stop() noexcept;

    void on_login(const std::string& msg);
    void on_command(const std::string& msg);
    void on_exit();

    void on_ping() const noexcept;
    bool timed_out() const noexcept;

    void write(const std::string& msg);

private:
    ip::tcp::socket socket_;
    std::string username_;
    size_t already_read_ {0};
    char buffer_[MAX_MSG] {0};

    win_process process_;

    chrono::time_point<chrono::steady_clock> last_ping_;
};


#endif //SERVER_CLIENT_SESSION_HPP
