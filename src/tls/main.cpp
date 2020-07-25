#include "../IPPacket.h"
#include "../TCPSocket.h"
#include "TLSHandshakeMessage.h"
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

  TLSHandshakeMessage client_hello{};
  std::cout << "Client key share: "
            << client_hello.find_key_share()->to_hex_byte_string() << '\n';

  auto payload_ptr{client_hello.get_raw()};
  TLSRecord record{ContentType_HANDSHAKE, 0x0301, *payload_ptr};
  socket.send_raw_bytes(*record.get_raw());

  std::unique_ptr<TLSRecord> server_hello_record{readTLSRecord(socket)};
  TLSHandshakeMessage server_hello{server_hello_record->get_fragment()};

  std::cout << server_hello << '\n';
  std::cout << "Server key share: "
            << server_hello.find_key_share()->to_hex_byte_string() << '\n';
  return 0;
}