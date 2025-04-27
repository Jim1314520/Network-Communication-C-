#pragma once
#include "socket.h"
#include <string>
namespace hy{
  namespace socket{
    class clientsocket : public socket{
      public:
      clientsocket() = delete;
      clientsocket(const string & ip, int port);
      clientsocket(int sockfd); 
      ~clientsocket() = default;
      string m_client_ip;
      int m_client_port;
      clientsocket(int sockfd, const string& ip, int port);

    };
  }


}