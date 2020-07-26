#include "DiffieHellman.h"
#include "../crypto/Curve25519.h"

KeyPair DiffieHellman::generate_key_pair(std::mt19937 &mersenne_twister) {
  FieldElement private_key{mersenne_twister};
  FieldElement base_point{9};
  FieldElement public_key{X25519(private_key, base_point)};

  return KeyPair(private_key, public_key);
}

FieldElement DiffieHellman::derive_shared_secret(
    const FieldElement &our_secret_key, const FieldElement &their_public_key) {
  return X25519(our_secret_key, their_public_key);
}
