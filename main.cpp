#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>

#include "ICMPSocket.h"
#include "UDPSocket.h"

int main() {
  UDPSocket send_socket{};
  ICMPSocket receive_socket{};

  std::string destination{"172.217.19.78"};
  int port{80};

  send_socket.send_string("hello", "172.217.19.78", 80);
  ICMPMessage received_msg{receive_socket.read_next()};
  std::cout << received_msg;

  send_socket.send_string("hello", "172.217.19.78", 80);
  received_msg = receive_socket.read_next();
  std::cout << received_msg;

  send_socket.send_string("hello", "172.217.19.78", 80);
  received_msg = receive_socket.read_next();
  std::cout << received_msg;
  return 0;
}
