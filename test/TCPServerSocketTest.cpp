#include <future>
#include "TCPServerSocket.h"
#include "gtest/gtest.h"

TEST(TCPServerSocketTest, simple_echo) {
  int server_port{8080};
  TCPServerSocket server_socket{server_port};
  server_socket.accept_connections();

  {
    TCPSocket client_socket{};
    client_socket.connect("127.0.0.1", server_port);
    std::string message{"Test message to be echoed back"};
    client_socket << message;
    auto bytes_ptr = client_socket.read_bytes(64);

    std::string response{bytes_ptr->get_buffer()};

    EXPECT_EQ(response, message);
  }

  server_socket.shutdown();
}

TEST(TCPServerSocketTest, connecting_several_times) {
  int server_port{8080};
  TCPServerSocket server_socket{server_port};
  server_socket.accept_connections();

  for (int i{0}; i < 10; ++i) {
    TCPSocket client_socket{};
    client_socket.connect("127.0.0.1", server_port);
    std::string message{"Test message to be echoed back"};
    client_socket << message;
    auto bytes_ptr = client_socket.read_bytes(64);

    std::string response{bytes_ptr->get_buffer()};

    EXPECT_EQ(response, message);
  }

  server_socket.shutdown();
}

TEST(TCPServerSocketTest, simultaneous_connections) {
  int server_port{8080};
  TCPServerSocket server_socket{server_port};
  server_socket.accept_connections();

  {
    TCPSocket client_one{};
    TCPSocket client_two{};
    client_one.connect("127.0.0.1", server_port);
    client_two.connect("127.0.0.1", server_port);
    std::string message_one{"Test message from client one"};
    std::string message_two{"Test message from client two"};
    client_one << message_one;
    client_two << message_two;
    auto bytes_ptr_one = client_one.read_bytes(64);
    auto bytes_ptr_two = client_two.read_bytes(64);

    std::string response_one{bytes_ptr_one->get_buffer()};
    std::string response_two{bytes_ptr_two->get_buffer()};

    EXPECT_EQ(response_one, message_one);
    EXPECT_EQ(response_two, message_two);
  }

  server_socket.shutdown();
}
