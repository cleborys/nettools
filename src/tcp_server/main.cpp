#include "../TCPServerSocket.h"

int main() {
  TCPServerSocket server_socket{8080};
  server_socket.accept_connections();

  return 0;
}
