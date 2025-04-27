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
        int accept();
        int send(const char * buf, int len);
        bool set_send_buffer(int size);

        int recv(char * buf,int len);
        void close();
        bool set_non_blocking();
        bool set_recv_buffer(int size);
        bool set_linger(bool active, int seconds);
        bool set_keepalive();
        bool set_reuseaddr();

      protected:
        string m_ip;
        int m_port;
        int m_sockfd;
    };

  }
}