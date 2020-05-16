#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include "Traceroute.h"

int main() {
  Traceroute tracer;
  std::string destination{"172.217.19.78"};

  for (int i{1}; i < 15; ++i) {
    tracer.ping("172.217.19.78", i);
    tracer.read_until_empty();
  }
  tracer.read_until_empty(1);

  tracer.print_records();
  return 0;
}
