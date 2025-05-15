#pragma once
#include "socket.h"
namespace hy{
  namespace socket{
    class serversocket : public socket{
      public:
      serversocket() = delete; // // 禁用默认构造函数，必须指定 IP/端口
      serversocket(const string & ip, int port);
      ~serversocket() = default;
    };
  } // namespace socket
}// namespace hy