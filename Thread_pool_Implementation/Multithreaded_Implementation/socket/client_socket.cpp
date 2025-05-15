#include "client_socket.h"

using namespace hy::socket;

clientsocket::clientsocket(const string & ip, int port) :socket(){
  connect(ip, port);
}

clientsocket::clientsocket(int sockfd)
    : socket(sockfd)    // 🔥 调用父类 socket(int sockfd)
{
}
 

clientsocket::clientsocket(int sockfd, const string& ip, int port)
    : socket(sockfd), m_client_ip(ip), m_client_port(port)
{
}
