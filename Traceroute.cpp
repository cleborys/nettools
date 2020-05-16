#include "Traceroute.h"
#include <unistd.h>
#include <iostream>

const size_t ICMP_HEADER_LENGTH{8};

void Traceroute::read_until_empty(int timeout_seconds) {
  while (m_socket.messages_available(timeout_seconds)) {
    auto time_received{std::chrono::high_resolution_clock::now()};

    std::unique_ptr<const RawBytes> received_bytes{m_socket.read_next()};
    if (received_bytes->actual_size <= 0) {
      continue;
    }
    std::unique_ptr<const ICMPMessage> parsed_message{
        parse_at(*received_bytes, 0)};

    if (parsed_message->msgtype == ICMP_TIME_EXCEEDED) {
      size_t body_start{parsed_message->ip_header.byte_length +
                        ICMP_HEADER_LENGTH};
      std::unique_ptr<const ICMPMessage> undelivered_message{
          parse_at(*received_bytes, body_start)};

      if (undelivered_message->identifier != getpid()) {
        continue;
      }

      auto &record_ptr{m_records[undelivered_message->sequence_number]};
      record_ptr->time_received = time_received;
      record_ptr->reply = std::move(parsed_message);
    } else if (parsed_message->msgtype == ICMP_ECHOREPLY) {
      auto &record_ptr{m_records[parsed_message->sequence_number]};
      record_ptr->time_received = time_received;
      record_ptr->reply = std::move(parsed_message);
    }
  }
}

void Traceroute::print_records() {
  for (auto const &entry : m_records) {
    const std::unique_ptr<Record> &record_ptr{entry.second};
    std::cout << "To " << record_ptr->destination
              << " with ttl=" << record_ptr->ttl << " received ";
    if (record_ptr->reply) {
      auto delay_ms{std::chrono::duration_cast<std::chrono::milliseconds>(
          record_ptr->time_received - record_ptr->time_sent)};
      std::cout << "reply from " << record_ptr->reply->ip_header.source
                << " after " << delay_ms.count() << "ms.\n";
    } else {
      std::cout << "no reply.\n";
    }
  }
}

void Traceroute::ping(std::string destination_ip, int ttl) {
  int ping_id{++last_ping_id};
  m_socket.set_ttl(ttl);
  auto echo_request_ptr{generate_echo_request(ping_id, getpid())};
  m_socket.send_raw(echo_request_ptr.get(), sizeof(*echo_request_ptr),
                    destination_ip, 33434);
  auto time_sent{std::chrono::high_resolution_clock::now()};

  auto record{std::make_unique<Record>()};
  record->time_sent = time_sent;
  record->destination = destination_ip;
  record->ttl = ttl;
  record->sequence_number = ping_id;

  m_records.insert(std::make_pair(ping_id, std::move(record)));
}
