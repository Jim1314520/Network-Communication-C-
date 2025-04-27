#pragma once
#include <string>

using std::string;

namespace hy{

  namespace socket{
    class socket{
      public:
        socket();
        socket(int sockfd);
        virtual ~socket();
        bool bind(const string & ip, int port);
        bool listen(int backlog);
        bool connect(const string & ip, int port);
        int accept(string& client_ip, int& client_port);

        //int accept();
        int send(const char * buf, int len);
        bool set_send_buffer(int size);

        int recv(char * buf,int len);
        void close();
        bool set_non_blocking();
        bool set_recv_buffer(int size);
        bool set_linger(bool active, int seconds);
        bool set_keepalive();
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