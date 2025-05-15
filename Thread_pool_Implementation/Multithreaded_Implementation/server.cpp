#include <iostream>
#include <signal.h>
#include <thread>
#include <functional>
#include <string>


#include "socket/server_socket.h"
#include "socket/client_socket.h"
#include "socket/thread_pool.h"
using namespace hy::socket;

void handle_session(int connfd, const std::string& client_ip, int client_port)
{
    std::cout << "[Worker] Handling new connection: " << client_ip << ":" << client_port 
          << ", Thread ID: " << std::this_thread::get_id() << std::endl;
    clientsocket cli(connfd, client_ip, client_port);
    char buf[1024];

    while (true) {
        int n = cli.recv(buf, sizeof(buf));
        if (n <= 0) {
            std::cout << "Client disconnected: " << client_ip << ":" << client_port 
                      << ", FD: " << connfd << std::endl;
            break;
        }

        buf[n] = '\0';
        std::cout << "Received from [" << client_ip << ":" << client_port << "] : " 
                  << buf << std::endl;

        cli.send(buf, n);
    }
}

int main()
{
    signal(SIGPIPE, SIG_IGN);

    serversocket server("127.0.0.1", 8080);
    ThreadPool thread_pool(8);   // 🔥 创建固定8线程的线程池
    
    while (true) {
        std::string client_ip;
        int client_port = 0;
        int connfd = server.accept(client_ip, client_port);
        if (connfd < 0) {
            continue;
        }

        std::cout << "New client connected: " << client_ip << ":" << client_port 
                  << ", FD: " << connfd << std::endl;

        // 🔥 把处理任务丢到线程池
        thread_pool.enqueue([connfd, client_ip, client_port] {
            handle_session(connfd, client_ip, client_port);
        });
    }

    server.close();
    return 0;
}
