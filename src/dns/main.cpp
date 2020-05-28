#include "../Socket.h"
#include "DNSQuerier.h"

int main() {
  DNSQuerier query_interface;
  query_interface.query("www.google.com.");

  return 0;
}
