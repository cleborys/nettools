#include "TCPSocket.h"
#include "gtest/gtest.h"

TEST(TCPSocketTest, connect_send_receive) {
  TCPSocket socket{};
  socket.connect("172.217.19.68", 80);
  socket << "GET /index.html HTTP/1.1\n\n";
  auto bytes_ptr = socket.read_bytes(15);

  std::string response{bytes_ptr->get_buffer()};
  std::string expected_response{"HTTP/1.1 200 OK"};

  EXPECT_EQ(response, expected_response);
}
