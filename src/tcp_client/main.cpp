#include "../TCPSocket.h"

int main() {
  TCPSocket socket{};

  socket.connect("172.217.19.68", 80);
  socket << "GET /index.html HTTP/1.1\n\n";
  auto bytes_ptr = socket.read_bytes(15);
  std::cout << bytes_ptr->get_buffer();
  return 0;
}