#pragma once
#include "socket.h"
namespace hy{
  namespace socket{
    class serversocket : public socket{
      public:
      serversocket() = delete;
      serversocket(const string & ip, int port);
      ~serversocket() = default;


    };
  }


}