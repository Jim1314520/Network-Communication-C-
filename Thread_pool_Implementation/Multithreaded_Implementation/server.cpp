#include <iostream>
#include <signal.h>
#include <thread>
#include <functional>
#include <string>
#include <cstring>
#include <chrono>

#include "include/server_socket.h"
#include "include/client_socket.h"
#include "include/thread_pool.h"
using namespace hy::socket;

void handle_session(int connfd, const std::string& client_ip, int client_port)
{
    auto start = std::chrono::steady_clock::now();
    std::cout << "[Worker] Handling connection: " << client_ip << ":" << client_port 
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
        cli.send(buf, n);
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "[Worker] Connection " << client_ip << ":" << client_port 
              << " processed in " << duration << " ms" << std::endl;
}

int main(int argc, char* argv[])
{
    // 忽略 SIGPIPE 信号，防止写断开连接导致进程终止
    signal(SIGPIPE, SIG_IGN);

    bool use_pool = false;
    if (argc > 1 && std::strcmp(argv[1], "--pool") == 0) {
        use_pool = true;
    }
    std::cout << "Starting server in " << (use_pool ? "thread-pool" : "no-pool") << " mode." << std::endl;

    serversocket server("127.0.0.1", 8080);

    ThreadPool thread_pool(8);

    while (true) {
        std::string client_ip;
        int client_port = 0;
        int connfd = server.accept(client_ip, client_port);
        if (connfd < 0) continue;

        std::cout << "New client connected: " << client_ip << ":" << client_port 
                  << ", FD: " << connfd << std::endl;

        if (use_pool) {
            thread_pool.enqueue([connfd, client_ip, client_port]{
                handle_session(connfd, client_ip, client_port);
            });
        } else {
            // 单连接单线程模式
            std::thread([connfd, client_ip, client_port] {
                handle_session(connfd, client_ip, client_port);
            }).detach();
        }
    }

    server.close();
    return 0;
}
