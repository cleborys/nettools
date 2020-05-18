#ifndef NETTOOLS__TRACEROUTE_H_
#define NETTOOLS__TRACEROUTE_H_

#include <map>
#include <memory>
#include <vector>
#include "Socket.h"
#include "packets.h"

class Traceroute {
 public:
  void read_until_empty(int timeout_seconds = 0);
  void print_records();
  void ping(std::string destination_ip, int ttl);

 private:
  struct SentRecord {
    std::chrono::time_point<std::chrono::high_resolution_clock> time{};
    std::string destination{};
    int ttl{};
    int sequence_number{};
  };

  struct ReceivedRecord {
    std::chrono::time_point<std::chrono::high_resolution_clock> time{};
    std::unique_ptr<const ICMPMessage> reply{nullptr};
  };

  std::map<int, std::unique_ptr<const SentRecord>> m_sent;
  std::map<int, std::unique_ptr<const ReceivedRecord>> m_received;

  ICMPSocket m_socket;
  int last_ping_id{0};
};

#endif  // NETTOOLS__TRACEROUTE_H_
