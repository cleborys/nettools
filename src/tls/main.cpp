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

  RawBytes raw_client_hello{
      "01 00 01 fc 03 03 df 2a 71 c3 76 9a 69 e3 76 fd 0b f8 6b 31 06 36 92 38 "
      "aa 17 57 67 ed a9 51 71 a1 a3 af 61 1f e5 20 39 63 d4 97 0d a7 10 f3 69 "
      "66 71 69 d4 05 0b 41 d3 0b b5 cc 23 8a 77 20 71 e2 83 26 d5 36 55 3a 00 "
      "24 13 01 13 03 13 02 c0 2b c0 2f cc a9 cc a8 c0 2c c0 30 c0 0a c0 09 c0 "
      "13 c0 14 00 33 00 39 00 2f 00 35 00 0a 01 00 01 8f 00 00 00 14 00 12 00 "
      "00 0f 73 73 6c 2e 67 73 74 61 74 69 63 2e 63 6f 6d 00 17 00 00 ff 01 00 "
      "01 00 00 0a 00 0e 00 0c 00 1d 00 17 00 18 00 19 01 00 01 01 00 0b 00 02 "
      "01 00 00 10 00 0e 00 0c 02 68 32 08 68 74 74 70 2f 31 2e 31 00 05 00 05 "
      "01 00 00 00 00 00 33 00 6b 00 69 00 1d 00 20 7e 97 1b 66 0b 9d fb 95 5e "
      "8c 2d 5e 7f 40 d7 e4 24 6a 19 02 14 58 d2 e8 28 14 02 fe bd 17 33 18 00 "
      "17 00 41 04 3a 1c 3f d3 25 2e 61 56 a8 91 c5 96 14 a0 ff 78 32 3b bb 69 "
      "6e c2 4c 40 d3 a7 4f 39 e1 30 ee 98 98 84 30 82 79 71 32 c4 5f 06 e0 1d "
      "c9 e0 19 2c ca bc e1 a1 32 3b ce 48 cd cb 32 8e 9f 69 4a 3a 00 2b 00 09 "
      "08 03 04 03 03 03 02 03 01 00 0d 00 18 00 16 04 03 05 03 06 03 08 04 08 "
      "05 08 06 04 01 05 01 06 01 02 03 02 01 00 1c 00 02 40 01 00 15 00 99 00 "
      "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00"};

  TLSClientHello client_hello{};
  auto payload_ptr{client_hello.get_raw()};
  TLSRecord record{ContentType_HANDSHAKE, 0x0301, *payload_ptr};

  socket.send_raw_bytes(*record.get_raw());
  // auto bytes_ptr = socket.read_bytes(15);
  // std::cout << bytes_ptr->get_buffer();
  return 0;
}