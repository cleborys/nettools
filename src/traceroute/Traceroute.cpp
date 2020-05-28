#include "Traceroute.h"
#include <unistd.h>
#include <iostream>

void Traceroute::read_until_empty(int timeout_seconds) {
  while (m_socket.messages_available(timeout_seconds)) {
    auto time_received{std::chrono::high_resolution_clock::now()};

    std::unique_ptr<const RawBytes> received_bytes{m_socket.read_next()};
    if (received_bytes->actual_size <= 0) {
      continue;
    }
    std::unique_ptr<const ICMPMessage> parsed_message{
        parse_icmp_at(*received_bytes, 0)};

    int sequence_number{-1};
    if (parsed_message->msgtype == ICMP_TIME_EXCEEDED) {
      size_t body_start{parsed_message->ip_header.byte_length +
                        ICMP_HEADER_LENGTH};
      std::unique_ptr<const ICMPMessage> undelivered_message{
          parse_icmp_at(*received_bytes, body_start)};

      if (undelivered_message->identifier != getpid()) {
        continue;
      }

      sequence_number = undelivered_message->sequence_number;
    } else if (parsed_message->msgtype == ICMP_ECHOREPLY) {
      sequence_number = parsed_message->sequence_number;
    }

    if (sequence_number != -1) {
      auto record_ptr{std::make_unique<ReceivedRecord>()};
      record_ptr->reply = std::move(parsed_message);
      record_ptr->time = time_received;
      m_received.insert(std::make_pair(sequence_number, std::move(record_ptr)));
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

  auto record{std::make_unique<SentRecord>()};
  record->time = time_sent;
  record->destination = destination_ip;
  record->ttl = ttl;
  record->sequence_number = ping_id;

  m_sent.insert(std::make_pair(ping_id, std::move(record)));
}

// LCOV_EXCL_START
void Traceroute::print_records() {
  for (auto const &entry : m_sent) {
    const std::unique_ptr<const SentRecord> &sent_record_ptr{entry.second};
    std::cout << "To " << sent_record_ptr->destination
              << " with ttl=" << sent_record_ptr->ttl << " received ";

    auto iterator{m_received.find(sent_record_ptr->sequence_number)};
    if (iterator != m_received.end()) {
      auto &received_record_ptr{iterator->second};
      auto delay_ms{std::chrono::duration_cast<std::chrono::milliseconds>(
          received_record_ptr->time - sent_record_ptr->time)};
      std::cout << "reply from " << received_record_ptr->reply->ip_header.source
                << " after " << delay_ms.count() << "ms.\n";
    } else {
      std::cout << "no reply.\n";
    }
  }
}
// LCOV_EXCL_STOP
