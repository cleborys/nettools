#ifndef NETTOOLS_DIFFIEHELLMAN_H
#define NETTOOLS_DIFFIEHELLMAN_H

#include "../crypto/FieldElement.h"

struct KeyPair {
  FieldElement private_key;
  FieldElement public_key;

  KeyPair(const FieldElement &private_key, const FieldElement &public_key)
      : private_key{private_key}, public_key{public_key} {};
};

class DiffieHellman {
 public:
  static KeyPair generate_key_pair(std::mt19937 &mersenne_twister);
  static FieldElement derive_shared_secret(
      const FieldElement &our_secret_key, const FieldElement &their_public_key);
};

#endif  // NETTOOLS_DIFFIEHELLMAN_H
