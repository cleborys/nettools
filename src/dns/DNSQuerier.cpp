#include "DNSQuerier.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "dns.h"

void DNSQuerier::query(const std::string &domain_name) {
  send_query(domain_name);
  parse_reply(domain_name.length());
}

void DNSQuerier::send_query(const std::string &domain_name) {
  unsigned char buffer[65536];

  DnsHeader *dns_header = reinterpret_cast<DnsHeader *>(buffer);
  dns_header->id = getpid();
  dns_header->query_response_flag = 0;  // 0 for query
  dns_header->opcode = 0;               // 0 for standard
  dns_header->authoritative_answer = 0;
  dns_header->truncated_message = 0;
  dns_header->recursion_desired = 1;
  dns_header->recursion_available = 0;
  dns_header->z_flag = 0;
  dns_header->authenticated_data = 0;
  dns_header->checking_disabled = 0;
  dns_header->response_code = 0;
  dns_header->question_count = htons(1);
  dns_header->answer_count = 0;
  dns_header->additional_record_count = 0;

  unsigned char *question_name = buffer + sizeof(DnsHeader);
  write_hostname(domain_name, question_name);

  QueryInfo *question_info = reinterpret_cast<QueryInfo *>(
      buffer + sizeof(DnsHeader) + (domain_name.size() + 1));
  question_info->question_class = htons(1);  // 1 for internet
  question_info->query_type = htons(1);      // 1 for A (IPv4)

  std::size_t message_length{sizeof(DnsHeader) + (domain_name.size() + 1) +
                             sizeof(QueryInfo)};
  m_socket.send_raw(buffer, message_length, "8.8.8.8", 53);
}

void DNSQuerier::parse_reply(int domain_name_length) {
  auto received_bytes = m_socket.read_next();
  const DnsHeader *received_dns_header =
      reinterpret_cast<const DnsHeader *>(received_bytes->buffer.get());

  const unsigned char *reader = reinterpret_cast<unsigned char *>(
      received_bytes->buffer.get() + sizeof(DnsHeader) +
      (domain_name_length + 1) + sizeof(QueryInfo));
  const unsigned char *buffer_start =
      reinterpret_cast<unsigned char *>(received_bytes->buffer.get());

  std::string name{};
  auto advance_count = copy_dns_name(reader, buffer_start, name);

  reader += advance_count;
  const ResourceRecordHeader *record_header =
      reinterpret_cast<const ResourceRecordHeader *>(reader);

  reader += sizeof(ResourceRecordHeader);
  unsigned char rdata[64];
  for (int i{0}; i < ntohs(record_header->resource_data_length); ++i) {
    rdata[i] = reader[i];
  }
  rdata[ntohs(record_header->resource_data_length)] = '\0';

  sockaddr_in addr{};
  addr.sin_addr.s_addr = *reinterpret_cast<long *>(rdata);
  std::cout << name << " resolved to " << inet_ntoa(addr.sin_addr) << '\n';
}
