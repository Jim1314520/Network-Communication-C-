#include "server_socket.h"
using namespace hy::socket;
#include <iostream>
#include <stdexcept>


serversocket::serversocket(const string & ip, int port) : socket() {
    set_recv_buffer(10 * 1024);
    set_send_buffer(10 * 1024);
    set_linger(true, 0);
    set_keepalive();
    set_reuseaddr();
    if (!bind(ip, port) || !listen(1024)) {
        std::cerr << "❌ Server socket initialization failed." << std::endl;
        close();
        throw std::runtime_error("Server socket failed to initialize.");
    }
}
