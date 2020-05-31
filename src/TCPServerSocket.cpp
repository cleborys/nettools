#include "TCPServerSocket.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

TCPServerSocket::TCPServerSocket(int port) : TCPSocket() {
  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(static_cast<uint16_t>(port));

  int opt = 1;
  if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                 sizeof(opt)) < 0) {
    std::cerr << "Failed to set socket option to allow multiple connections";
    throw std::runtime_error(
        "Failed to set socket option to allow multiple connections");
  }

  if (bind(m_socket, reinterpret_cast<sockaddr *>(&server_address),
           sizeof(server_address)) < 0) {
    std::cerr << "Error binding on port " << port << '\n';
    throw std::runtime_error("Error binding to port");
  }
  std::cout << "Bound to localhost:" << port << '\n';
}

void TCPServerSocket::accept_connections() {
  if (connection_event_loop) {
    std::cout << "Already running.\n";
    return;
  }
  listen(m_socket, 5);
  std::cout << "Listening...\n";

  running = true;
  connection_event_loop = std::make_unique<std::future<void>>(
      std::async(&TCPServerSocket::handle_connection_events, this));
}

void TCPServerSocket::shutdown() {
  running = false;
  if (connection_event_loop) {
    connection_event_loop->wait();
    connection_event_loop.reset();
  }
}

void TCPServerSocket::handle_connection_events() {
  fd_set connection_descriptors;
  timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  while (running) {
    FD_ZERO(&connection_descriptors);

    FD_SET(m_socket, &connection_descriptors);
    int max_descriptor{m_socket};
    for (int client_socket : client_sockets) {
      if (client_socket == 0) {
        continue;
      }

      FD_SET(client_socket, &connection_descriptors);
      if (client_socket > max_descriptor) {
        max_descriptor = client_socket;
      }
    }

    // block until a connection has activity, timeout to check if still running
    if (select(max_descriptor + 1, &connection_descriptors, nullptr, nullptr,
               &timeout) < 0) {
      std::cerr << "Error selecting next active connection";
      throw std::runtime_error("Error selecting next active connection");
    }

    // handle any newly established connections
    if (FD_ISSET(m_socket, &connection_descriptors)) {
      handle_new_connection();
    }

    // handle any existing connections with activity
    for (int &client_socket : client_sockets) {
      if (client_socket == 0) {
        continue;
      }

      if (FD_ISSET(client_socket, &connection_descriptors)) {
        handle_existing_connection(client_socket);
      }
    }
  }
  std::cout << "Shutting down.\n";
}

void TCPServerSocket::handle_new_connection() {
  sockaddr_in client_address;
  int client_address_length{sizeof(client_address)};
  int new_client_socket{
      accept(m_socket, reinterpret_cast<sockaddr *>(&client_address),
             reinterpret_cast<socklen_t *>(&client_address_length))};
  if (new_client_socket < 0) {
    std::cerr << "Error accepting a connection";
    throw std::runtime_error("Error accepting a connection");
  }
  std::cout << "Accepted a new connection to "
            << inet_ntoa(client_address.sin_addr) << ':'
            << ntohs(client_address.sin_port) << " on file descriptor "
            << new_client_socket << '\n';

  // Add to existing connections
  for (int &client_socket : client_sockets) {
    if (client_socket == 0) {
      client_socket = new_client_socket;
      return;
    }
  }

  // Reached max open connections
  close(new_client_socket);
}

void TCPServerSocket::handle_existing_connection(int &client_socket) {
  RawBytes read_data{1024};
  std::size_t read_length{static_cast<size_t>(
      read(client_socket, read_data.get_buffer(), read_data.max_size))};

  if (read_length == 0) {  // Connection closed
    disconnect(client_socket);
    return;
  }

  // otherwise echo back what we received
  write(client_socket, read_data.get_buffer(), strlen(read_data.get_buffer()));
}

void TCPServerSocket::disconnect(int &client_socket) {
  sockaddr_in client_address;
  int client_address_length{sizeof(client_address)};
  getpeername(client_socket, reinterpret_cast<sockaddr *>(&client_address),
              reinterpret_cast<socklen_t *>(&client_address_length));
  std::cout << "Disconnected from " << inet_ntoa(client_address.sin_addr) << ':'
            << ntohs(client_address.sin_port) << " on file descriptor "
            << client_socket << '\n';
  close(client_socket);
  client_socket = 0;
}
