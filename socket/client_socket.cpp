#include "client_socket.h"
using namespace hy::socket;

clientsocket::clientsocket(const string & ip, int port) :socket(){
  connect(ip, port);

}