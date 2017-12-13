//
// Created by Namat on 31.07.2017.
//

#include "talk_to_client.hpp"

#include <iostream>

using namespace boost::asio;

void talk_to_client::read_request() {
    if(socket.available())
        already_read += socket.read_some(buffer(buff+already_read,max_msg - already_read));
}

void talk_to_client::process_request() {
    bool found_enter = std::find(buff, buff + already_read, '\n')
                       < buff + already_read;
    if ( !found_enter)
        return; // message is not full
    // process the msg
    last_ping = boost::posix_time::microsec_clock::local_time();
    size_t pos = std::find(buff, buff + already_read, '\n') - buff;
    std::string msg(buff, pos);
    std::copy(buff + already_read, buff + max_msg, buff);
    already_read -= pos + 1;

    if ( msg.find("login ") == 0) on_login(msg);
    else if ( msg.find("ping") == 0) on_ping();
    else if ( msg.find("ask_clients") == 0) on_clients();
    else std::cerr << "invalid msg " << msg << std::endl;
}

void talk_to_client::on_login(const std::string &msg) {
    std::istringstream in(msg);
    in >> username >> username;
    std::cout << username << " logged in" << std::endl;
    write("login ok\n");
    server.update_clients_changed();
}

void talk_to_client::on_ping() {
    write(clients_changed ? "ping client_list_changed\n" : "ping ok\n");
    clients_changed = false;
}

void talk_to_client::on_clients() {
    std::string msg;
    { std::scoped_lock<std::recursive_mutex> lk(server.mutex_);
        for(auto& client : server.clients_)
            msg += client->login() + " ";
    }
    write("clients_ " + msg + "\n");
}

void talk_to_client::write(const std::string &msg) {
    socket.write_some(buffer(msg));
}

talk_to_client::talk_to_client(sync_tcp_server& server) :
        server(server),socket(server.service_), started(false), already_read(0) {
    last_ping = boost::posix_time::microsec_clock::local_time();
}

std::string talk_to_client::login() const {
    return username;
}

void talk_to_client::answer_to_client() {
    try {
        read_request();
        process_request();
    } catch ( boost::system::system_error&) {
        stop();
    }
    if ( timed_out()) {
        stop();
        std::cout << "stopping " << username << " - no ping in time" << std::endl;
    }
}

void talk_to_client::set_clients_changed() {
    clients_changed = true;
}

ip::tcp::socket& talk_to_client::sock(){
    return socket;
}

bool talk_to_client::timed_out() const {
    auto now = boost::posix_time::microsec_clock::local_time();
    long long ms = (now - last_ping).total_milliseconds();
    return ms > 5000 ;
}

void talk_to_client::stop() {
    boost::system::error_code err;
    socket.close(err);
}
