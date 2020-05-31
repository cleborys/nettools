#include "../TCPServerSocket.h"

int main(int argc, char* argv[]) {
  int port{};
  if (argc != 2) {
    port = 8080;
    std::cout << "Usage: " << *argv << " <port>.\n"
              << "Will use port " << port << " by default.\n";

  } else {
    port = atoi(argv[1]);
  }

  TCPServerSocket server_socket{port};
  server_socket.accept_connections();

  return 0;
}
