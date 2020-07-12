#include "../IPPacket.h"
#include "../TCPSocket.h"
#include "TLSClientHello.h"
#include "TLSRecord.h"

int main(int argc, char* argv[]) {
  std::string destination_ip{};
  int destination_port{};

  if (argc != 3) {
    std::cout << "Usage: " << *argv << " <IP> <port>\n";
    destination_ip = "216.58.213.195";
    destination_port = 443;
  } else {
    destination_ip = argv[1];
    destination_port = atoi(argv[2]);
  }

  TCPSocket socket{};

  socket.connect(destination_ip, destination_port);

  TLSClientHello client_hello{};
  auto payload_ptr{client_hello.get_raw()};
  TLSRecord record{ContentType_HANDSHAKE, 0x0301, *payload_ptr};

  socket.send_raw_bytes(*record.get_raw());
  auto bytes_ptr = socket.read_bytes(15);
  std::cout << bytes_ptr->get_buffer();
  std::cout << "enter to terminate\n";
  std::cin.get();
  return 0;
}