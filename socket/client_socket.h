#pragma once
#include "socket.h"
namespace hy{
  namespace socket{
    class clientsocket : public socket{
      public:
      clientsocket() = delete;
      clientsocket(const string & ip, int port);
      ~clientsocket() = default;


    };
  }


}