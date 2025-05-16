#include <iostream>
#include "client_socket.h"
using namespace hy::socket;
#include <thread>
int main() {
    clientsocket client("127.0.0.1", 8080);
    std::cout << "Connected to server.\n";
    // 自动发5条消息
    char buf[1024] = {0};
    for (int i = 0; i < 5; ++i) {
        std::string data = "Auto message " + std::to_string(i);
        client.send(data.c_str(), data.size());

        int n = client.recv(buf, sizeof(buf) - 1); // 最多接收 1023 字节，预留最后一个字节放 \0
        if (n > 0) {
            buf[n] = '\0';
            std::cout << "[Server Echo]: " << buf << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // 每条间隔1秒
    }
    // 发送完后保持连接，防止退出
    std::cout << "All messages sent. Keeping connection open...\n";

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}
