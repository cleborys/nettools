#include <ctime>
#include <random>
#include "../IPPacket.h"
#include "../TCPSocket.h"
#include "../crypto/Curve25519.h"
#include "../crypto/FieldElement.h"
#include "DiffieHellman.h"
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

  std::mt19937 mersenne_twister{
      static_cast<std::mt19937::result_type>(std::time(nullptr))};
  KeyPair client_key_pair{DiffieHellman::generate_key_pair(mersenne_twister)};
  KeyShare client_key_share{client_key_pair.public_key};

  std::cout << "Client private key: " << client_key_pair.private_key.to_string()
            << '\n';
  std::cout << "Client public key: \t" << client_key_pair.public_key.to_string()
            << '\n';

  TCPSocket socket{};

  socket.connect(destination_ip, destination_port);

  TLSHandshakeMessage client_hello{client_key_share};
  std::cout << "Client key share: \t"
            << client_hello.find_key_share()->to_hex_byte_string() << '\n';

  auto payload_ptr{client_hello.get_raw()};
  TLSRecord record{ContentType_HANDSHAKE, 0x0301, *payload_ptr};
  socket.send_raw_bytes(*record.get_raw());

  std::unique_ptr<TLSRecord> server_hello_record{readTLSRecord(socket)};
  TLSHandshakeMessage server_hello{server_hello_record->get_fragment()};

  std::cout << server_hello << '\n';
  std::cout << "Server key share: \t"
            << server_hello.find_key_share()->to_hex_byte_string() << '\n';

  FieldElement shared_secret{DiffieHellman::derive_shared_secret(
      client_key_pair.private_key,
      FieldElement(server_hello.find_key_share()->to_hex_byte_string()))};
  std::cout << "Shared secret: \t\t" << shared_secret.to_string() << '\n';

  std::cout << std::dec;
  std::unique_ptr<TLSRecord> first_record{readTLSRecord(socket)};
  std::cout << "first record type: " << first_record->get_type()
            << " should be ChangeCipherSpec(20) and is simply dropped\n";
  std::unique_ptr<TLSRecord> second_record{readTLSRecord(socket)};
  std::cout << "second record type: " << second_record->get_type() << '\n';
  return 0;
}