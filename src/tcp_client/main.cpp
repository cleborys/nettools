#include "../TCPSocket.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << *argv << " <IP> <port>\n";
    return 1;
  }

  std::string destination_ip{argv[1]};
  int destination_port{atoi(argv[2])};
  TCPSocket socket{};

  socket.connect(destination_ip, destination_port);
  socket << "GET /index.html HTTP/1.1\n\n";
  auto bytes_ptr = socket.read_bytes(15);
  std::cout << bytes_ptr->get_buffer();
  return 0;
}