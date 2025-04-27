#include <iostream>
#include "socket/client_socket.h"

using namespace hy::socket;

int main() {
   
//     // 1. 创建 socket
//     socket client;

//     // 2. 配置服务器地址
//    if (!client.connect("127.0.0.1", 8080)) {  // 可能会发生阻塞
//     std::cerr << "Failed to connect to server.\n";
//     return -1;
//    }
    clientsocket client("127.0.0.1", 8080);
    // 3. 向服务端发送数据
    string data = "hello world";
    client.send(data.c_str(), data.size());   // 可能会发生阻塞

    // 4. 接受服务端的数据
    char buf[1024] = {0};
    client.recv(buf, data.size()); // 可能会发生阻塞
    std::cout << buf << std::endl;

    // 6. 关闭 socket
   // client.close();
   
    return 0;
}
