#ifndef NETTOOLS_DNSTRACE_DNSQUERIER_H_
#define NETTOOLS_DNSTRACE_DNSQUERIER_H_
#include "../Socket.h"

class DNSQuerier {
 public:
  std::string query(const std::string &domain_name);

 private:
  void send_query(const std::string &domain_name);
  std::string parse_reply(int domain_name_length);
  UDPSocket m_socket;
};

#endif  // NETTOOLS_DNSTRACE_DNSQUERIER_H_
