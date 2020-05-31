#ifndef NETTOOLS_SRC_TCPSERVERSOCKET_H_
#define NETTOOLS_SRC_TCPSERVERSOCKET_H_
#include <array>
#include <future>
#include "TCPSocket.h"

class TCPServerSocket : private TCPSocket {
 public:
  TCPServerSocket(int port);
  void accept_connections();
  void shutdown() { running = false; }

 private:
  std::array<int, 30> client_sockets{};
  std::atomic_bool running{false};

  void handle_new_connection();
  void handle_existing_connection(int &client_socket);
  void disconnect(int &client_socket);
};

#endif  // NETTOOLS_SRC_TCPSERVERSOCKET_H_
