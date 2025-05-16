#pragma once
#include <string>

using std::string;

namespace hy{

  namespace socket{
    // 基础 Socket 类，封装了 TCP 套接字常用操作
    class socket{
      public:
        socket();
        socket(int sockfd);
        virtual ~socket();
        bool bind(const string & ip, int port); 
        // 开始监听连接请求（仅服务端调用，backlog 表示最大排队数量）
        bool listen(int backlog);
        bool connect(const string & ip, int port);
         // 接收客户端连接（仅服务端使用），返回新的 socket fd，并输出客户端 IP 和端口
        int accept(string& client_ip, int& client_port);

        //int accept();
        int send(const char * buf, int len);
        bool set_send_buffer(int size);

        int recv(char * buf,int len);
        void close();
        bool set_non_blocking();
        bool set_recv_buffer(int size);
        bool set_linger(bool active, int seconds);
         // 开启 TCP keepalive 检测，防止死连接
        bool set_keepalive();
        // 设置端口复用（通常服务端重启时希望立即重新绑定端口）
        bool set_reuseaddr();

        int  get_fd() const { return m_sockfd; }   // 让外部拿到底层 fd
        bool is_valid() const { return m_sockfd >= 0; }   // 可选，后面检测用


      protected:
        string m_ip;
        int m_port;
        int m_sockfd;
    };

  }
}