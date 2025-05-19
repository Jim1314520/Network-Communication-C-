#include <iostream>
#include <fstream>
#include <signal.h>
#include <thread>
#include <functional>
#include <string>
#include <cstring>
#include <chrono>
#include <sys/socket.h>
#include "include/server_socket.h"
#include "include/client_socket.h"
#include "include/thread_pool.h"
using namespace hy::socket;

// ✅ 日志文件输出定义
std::ofstream log_file;
#define LOG(x) do { std::cout << x; log_file << x; } while(0)

void handle_session(int connfd, const std::string& client_ip, int client_port)
{
    auto start = std::chrono::steady_clock::now();
    LOG("[Worker] Handling connection: " << client_ip << ":" << client_port 
        << ", Thread ID: " << std::this_thread::get_id() << std::endl);

    clientsocket cli(connfd, client_ip, client_port);

    // ✅ 设置接收超时时间（5 秒）
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(cli.get_fd(), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buf[1024];
    while (true) {
        int n = cli.recv(buf, sizeof(buf));
        if (n <= 0) {
            LOG("Client disconnected or timeout: " << client_ip << ":" << client_port 
                << ", FD: " << connfd << std::endl);
            break;
        }
        buf[n] = '\0';
        cli.send(buf, n);
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    LOG("[Worker] Connection " << client_ip << ":" << client_port 
        << " processed in " << duration << " ms" << std::endl);
}

int main(int argc, char* argv[])
{
    // 忽略 SIGPIPE 信号，防止写断开连接导致进程终止
    signal(SIGPIPE, SIG_IGN);

    bool use_pool = false;
    if (argc > 1 && std::strcmp(argv[1], "--pool") == 0) {
        use_pool = true;
    }

    // ✅ 动态打开日志文件
    log_file.open(use_pool ? "server_pool.log" : "server_no_pool.log");

    LOG("Starting server in " << (use_pool ? "thread-pool" : "no-pool") << " mode." << std::endl);

    serversocket server("127.0.0.1", 8080);
    ThreadPool thread_pool(8);

    while (true) {
        std::string client_ip;
        int client_port = 0;
        int connfd = server.accept(client_ip, client_port);
        if (connfd < 0) continue;

        LOG("New client connected: " << client_ip << ":" << client_port 
            << ", FD: " << connfd << std::endl);

        if (use_pool) {
            thread_pool.enqueue([connfd, client_ip, client_port]{
                handle_session(connfd, client_ip, client_port);
            });
        } else {
            std::thread([connfd, client_ip, client_port] {
                handle_session(connfd, client_ip, client_port);
            }).detach();
        }
    }

    server.close();
    log_file.close();
    return 0;
}
