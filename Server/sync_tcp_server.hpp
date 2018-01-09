//
// Created by Namat on 31.07.2017.
//

#ifndef SERVER_SYNC_TCP_SERVER_HPP
#define SERVER_SYNC_TCP_SERVER_HPP

#include <boost/asio.hpp>
#include "client_session.hpp"

static constexpr int PORT = 8001;

namespace asio = boost::asio;

/* Простой синхронный сервер:
    - клиенты регистрируются только с именем пользователя (без пароля)
    - на каждого клиента выделяется поток
    - все подключения инициируются клиентом: клиент запрашивает, отвечает сервер
    - сервер отключает любого клиента, который не пинговал в течение 1 минуты
*/

class sync_tcp_server {
public:
    void start();
private:
    asio::io_service service_;
};


#endif //SERVER_SYNC_TCP_SERVER_HPP
