#include "socket.h"
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
using namespace hy::socket;
socket::socket(){
  m_sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(m_sockfd < 0){
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl; 
  } else{
    std::cout << "Socket created successfully. FD: " << m_sockfd << std::endl;
  }
}
socket::~socket() {
  close();
}

socket::socket(int sockfd)  : m_ip(""),m_port(0), m_sockfd(sockfd) {

}

bool socket::bind(const string & ip, int port){
    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr)); // 清空结构体
    sockaddr.sin_family = AF_INET;           // 使用 IPv4
    sockaddr.sin_port = htons(port);         // 设置端口（主机序转网络序）
    if(ip.empty()){
      sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else{
      sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());   // 任意本地地址（0.0.0.0）
    }
    // 3. 绑定地址和端口
    if (::bind(m_sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return false;
    } else {
        std::cout << "Bind successful on port 8080." << std::endl;
    }
    return true;
}
bool socket::listen(int backlog){
 if(::listen(m_sockfd, backlog) < 0){
      std::cerr << "Listen failed: " << strerror(errno) << std::endl;
      return false;
    } else{
       std::cout << "Listening for incoming connections..." << std::endl;
       return true;
  }
  
}

bool socket::connect(const string & ip, int port){
    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (::connect(m_sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}
int socket::accept(){
  int connfd = ::accept(m_sockfd, nullptr,nullptr ); 
  if (connfd < 0) {
      std::cerr << "Accept failed: " << strerror(errno) << std::endl;
      return -1;
  }
  return connfd;

}
int socket::send(const char * buf, int len){
  return ::send(m_sockfd, buf, len, 0);
}

int socket::recv(char * buf,int len){
  return ::recv(m_sockfd, buf, len, 0);
}

void socket::close(){
  if(m_sockfd >= 0){
    ::close(m_sockfd);
    m_sockfd = 0;
  }
}
bool socket::set_non_blocking(){
  int flags = fcntl(m_sockfd, F_GETFL,0);
  if(flags < 0) {
    std::cerr << "fcntl F_GETFL failed:" << strerror(errno) << std::endl;
    return false;
  }
  flags |= O_NONBLOCK;
  if(fcntl(m_sockfd, F_SETFL, flags) < 0){
    std::cerr << "fcntl F_SETFL failed: " << strerror(errno) << std::endl;
    return false;
  }
  std::cout << "Socket set to non-blocking mode." << std::endl;
  return true;
}

bool socket::set_send_buffer(int size){
  int buff_size = size;
  if(setsockopt(m_sockfd, SOL_SOCKET,  SO_SNDBUF,&buff_size, sizeof(buff_size)) < 0){
    std::cerr << "❌ Failed to set send buffer size: " << strerror(errno) << std::endl;
    return false;
  }
  std::cout << "✅ Send buffer size set to " << buff_size << " bytes" << std::endl;
  return true;
}

bool socket::set_recv_buffer(int size){
   if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) < 0) {
        std::cerr << "❌ Failed to set receive buffer size: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "✅ Receive buffer size set to " << size << " bytes" << std::endl;
    return true;
}
bool socket::set_linger(bool active, int seconds){
    struct linger ling;
    std::memset(&ling, 0, sizeof(ling));
    ling.l_onoff = active ? 1 : 0;
    ling.l_linger = seconds;

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0) {
        std::cerr << "❌ Failed to set SO_LINGER: " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "✅ SO_LINGER set: " 
              << (active ? ("enabled, " + std::to_string(seconds) + "s") : "disabled") 
              << std::endl;

    return true;
}
bool socket::set_keepalive(){
    int optval = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0) {
        std::cerr << "❌ Failed to set SO_KEEPALIVE: " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "✅ SO_KEEPALIVE enabled on socket." << std::endl;
    return true;
}

bool socket::set_reuseaddr(){
    int optval = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        std::cerr << "❌ Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "✅ SO_REUSEADDR enabled on socket." << std::endl;
    return true;
}



