#include "server_socket.h"
using namespace hy::socket;
serversocket ::serversocket(const string & ip, int port) :socket(){
  //set_non_blocking();
  set_recv_buffer(10 * 1024);
  set_send_buffer(10 * 1024);
  set_linger(true, 0);
  set_keepalive();
  set_reuseaddr();
  bind(ip, port);
  listen(1024);
}