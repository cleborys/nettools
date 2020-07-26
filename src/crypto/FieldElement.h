#ifndef NETTOOLS_SRC_TCP_SERVER_FIELDELEMENT_H_
#define NETTOOLS_SRC_TCP_SERVER_FIELDELEMENT_H_

#include <array>
#include <random>

/* Support for calculations in f25519
 * encoded as little endian arrays of size 32
 * (least significant byte first)
 *
 * Note that p = 2^255 - 19
 * is 0xFF bytes
 * except the LSB which is 0xed = 11101101 = 237
 * and the MSB which is 0x7F = 0b01111111 = 127
 * and 2p = (p << 1)
 * is all 0xFF bytes
 * except the LSB which is 0xda = 11011010
 */

constexpr size_t FIELD_SIZE{32};

class FieldElement {
  std::array<std::uint8_t, FIELD_SIZE> bytes;

 public:
  FieldElement(std::mt19937 &mersenne_twister);  // random value
  FieldElement(std::uint32_t value);
  FieldElement(const FieldElement &original);
  FieldElement(const std::string &hex_byte_string);
  FieldElement &operator=(const FieldElement &other);
  std::string to_string() const;
  bool operator==(const FieldElement &other) const;
  bool bit_at(size_t position) const;

  FieldElement normalize() const;  // subtract p until < p.
  FieldElement operator^(const FieldElement &other) const;
  FieldElement operator&(const FieldElement &other) const;
  FieldElement operator+(const FieldElement &other) const;
  FieldElement operator-() const;
  FieldElement operator-(const FieldElement &other) const;
  FieldElement operator<<(int shift) const;
  FieldElement operator>>(int shift) const;
  FieldElement operator*(const FieldElement &other) const;
  FieldElement pow(const FieldElement &exponent) const;
  FieldElement inverse() const;

  FieldElement mask_most_significant_bit() const;
  FieldElement prepare_scalar() const;  // see page 7 of RFC 7748
  bool is_zero_constant_time() const;

 private:
  void add_inplace_without_normalization(const FieldElement &other);
};

#endif  // NETTOOLS_SRC_TCP_SERVER_FIELDELEMENT_H_
