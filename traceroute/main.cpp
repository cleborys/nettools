#include <future>
#include <iostream>
#include <sstream>
#include <string>

#include "Traceroute.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << *argv << " <IP>  <max_hops>\n";
    return 1;
  }

  std::string destination{argv[1]};
  int max_hops{};
  std::istringstream conversion_stream{argv[2]};
  if (!(conversion_stream >> max_hops)) {
    std::cerr << "Could not convert max hops " << argv[2] << " to int.\n";
    return 1;
  }
  max_hops = std::max(0, std::min(max_hops, 255));

  Traceroute tracer;

  auto read_future = std::async(&Traceroute::read_until_empty, &tracer, 1);
  for (int i{1}; i <= max_hops; ++i) {
    tracer.ping(destination, i);
  }
  read_future.wait();

  tracer.print_records();
  return 0;
}
