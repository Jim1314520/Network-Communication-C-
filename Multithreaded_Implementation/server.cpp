#include <iostream>
#include <signal.h>
#include <thread>
#include "socket/server_socket.h"
#include "socket/client_socket.h"

using namespace hy::socket;

void handle_session(int connfd, const std::string& client_ip, int client_port)
{
    clientsocket cli(connfd, client_ip, client_port);
    char buf[1024];

    while (true) {
        int n = cli.recv(buf, sizeof(buf));
        if (n <= 0) {
            std::cout << "Client disconnected: " << client_ip << ":" << client_port 
                      << ", FD: " << connfd << std::endl;
            break;
        }

        buf[n] = '\0';  // 处理 recv 的数据，防止脏数据输出
        std::cout << "Received from [" << client_ip << ":" << client_port << "]: " 
                  << buf << std::endl;

        cli.send(buf, n);  // Echo回去
    }
}

int main()
{
    signal(SIGPIPE, SIG_IGN);        // 防止写已关闭 socket 崩溃

    serversocket server("127.0.0.1", 8080);

    while (true) {
        std::string client_ip;
        int client_port = 0;
        int connfd = server.accept(client_ip, client_port);   // 带IP/端口返回
        if (connfd < 0) {
            continue;
        }

        std::cout << "New client connected: " << client_ip << ":" << client_port 
                  << ", FD: " << connfd << std::endl;

        std::thread(handle_session, connfd, client_ip, client_port).detach();
    }

    server.close();
    return 0;
}
