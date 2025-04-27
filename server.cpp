#include <iostream>

#include "socket/server_socket.h"

using namespace hy::socket;
int main(){

  // // 1.创建socket
  
  // socket servsr;

  // // 3. 绑定地址和端口   

  // servsr.bind("127.0.0.1", 8080);

  // // 4. 监听 socket
   
  // servsr.listen(1024);

  serversocket server("127.0.0.1", 8080);
  while(true){
      int connfd = server.accept(); // 可能会发生阻塞
      if(connfd < 0) return 1;
      socket client(connfd);
      char buf[1024] = {0};
      size_t len = client.recv(buf, sizeof(buf)); // 可能会发生阻塞
      std::cout << "Received from client: " << buf << std::endl;
      client.send(buf, len); // 可能会发生阻塞
      client.close();
  }
  server.close();
  return 0;     
}

