#include "FieldElement.h"
#include <iomanip>
#include <sstream>

FieldElement::FieldElement(std::mt19937 &mersenne_twister) : bytes{} {
  std::uniform_int_distribution generator{0, 255};
  for (size_t walk{0}; walk < FIELD_SIZE; ++walk) {
    bytes[walk] = static_cast<std::uint8_t>(generator(mersenne_twister));
  }
}

FieldElement::FieldElement(std::uint32_t value) : bytes{} {
  size_t walk{0};
  while (value > 0) {
    bytes[walk++] = value;
    value >>= 8;
  }
}

FieldElement::FieldElement(const FieldElement &original)
    : bytes{original.bytes} {}

FieldElement::FieldElement(const std::string &hex_byte_string) {
  if (hex_byte_string.size() != 32 * 2 + 31) {
    throw std::runtime_error("String must be 32 space-separated bytes");
  }

  std::istringstream convert{hex_byte_string};
  std::uint16_t buffer{};  // necessary for >> to read two characters at once!
  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    convert >> std::hex >> buffer;
    bytes[i] = buffer;
  }
}

FieldElement &FieldElement::operator=(const FieldElement &other) {
  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    bytes[i] = other.bytes[i];
  }
  return *this;
}

std::string FieldElement::to_string() const {
  std::ostringstream convert{};
  for (std::uint8_t byte : bytes) {
    convert << std::noshowbase << std::hex << std::setfill('0') << std::setw(2);
    convert << static_cast<int>(byte) << ' ';
  }

  std::string return_string{convert.str()};
  if (return_string.size() > 0) {
    return_string.pop_back();
  }

  return return_string;
}

bool FieldElement::operator==(const FieldElement &other) const {
  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    if (bytes[i] != other.bytes[i]) {
      return false;
    }
  }
  return true;
}

bool FieldElement::bit_at(size_t position) const {
  position = position % 256;
  size_t byte_nbr{position / 8};
  size_t bit_nbr{position % 8};
  return ((bytes[byte_nbr] & (1 << bit_nbr)) != 0);
}

FieldElement FieldElement::normalize() const {
  FieldElement first_reduction{*this};

  // The most significant bit is 2^255 % p = 19
  // If it is set, we erase it and add 19.
  if (first_reduction.bytes[31] >= 0b10000000) {
    first_reduction.bytes[31] &= 0b01111111;
    first_reduction.add_inplace_without_normalization(FieldElement(19));
  }

  // Now first_reduction is at most 2^255 - 1 + 18
  // hence less than 2p.
  // So we subtract p and return the result if it didn't underflow
  // otherwise we return first_reduction.
  // That is, we add 19 and then check if the result is bigger than 2^255 = p+19
  // in which case we subtract 2^255 by unsetting the bit.

  FieldElement second_reduction{first_reduction};
  second_reduction.add_inplace_without_normalization(FieldElement(19));
  if (second_reduction.bytes[31] >= 0b10000000) {
    second_reduction.bytes[31] &= 0b01111111;
    return second_reduction;
  }
  return first_reduction;
}

void FieldElement::add_inplace_without_normalization(
    const FieldElement &other) {
  std::uint16_t carry{0};

  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    carry >>= 8;
    carry += bytes[i] + other.bytes[i];
    bytes[i] = carry;
  }
  // overflow carry is discarded
}

FieldElement FieldElement::operator^(const FieldElement &other) const {
  FieldElement result{*this};
  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    result.bytes[i] ^= other.bytes[i];
  }

  return result;
}

FieldElement FieldElement::operator&(const FieldElement &other) const {
  FieldElement result{*this};
  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    result.bytes[i] &= other.bytes[i];
  }

  return result;
}

FieldElement FieldElement::operator+(const FieldElement &other) const {
  FieldElement result{*this};
  result.add_inplace_without_normalization(other);
  return result.normalize();
}

FieldElement FieldElement::operator-() const {
  // Calculate 2p - *this,
  // then return the normalized result
  FieldElement result{0};

  std::uint32_t carry{0xda};  // least significant byte of 2p
  for (size_t i{0}; i < FIELD_SIZE - 1; ++i) {
    carry += 0xff00;  // next byte of 2p
    carry -= static_cast<std::uint32_t>(bytes[i]);
    result.bytes[i] = carry;  // discards overflow
    carry >>= 8;
  }
  // still have to set the most significant byte,
  // but now without adding the next byte of 2p
  carry -= static_cast<std::uint32_t>(bytes[31]);
  result.bytes[31] = carry;

  return result.normalize();
}

FieldElement FieldElement::operator-(const FieldElement &other) const {
  FieldElement negation{-other};
  FieldElement sum{*this + negation};
  return sum;
  // return *this + (-other);
}

FieldElement FieldElement::operator<<(int shift) const {
  if (shift < 0) {
    throw std::runtime_error("Can only shift by positive amounts");
  }

  FieldElement result{0};

  int byte_shift{shift / 8};
  int sub_shift{shift % 8};

  result.bytes[byte_shift] = bytes[0] << sub_shift;
  for (size_t i{1}; i + byte_shift < FIELD_SIZE; ++i) {
    result.bytes[i + byte_shift] =
        (bytes[i] << sub_shift) + (bytes[i - 1] >> (8 - sub_shift));
  }

  return result;
}

FieldElement FieldElement::operator>>(int shift) const {
  if (shift < 0) {
    throw std::runtime_error("Can only shift by positive amounts");
  }

  FieldElement result{0};

  int byte_shift{shift / 8};
  int sub_shift{shift % 8};

  for (size_t i{0}; i + 1 + byte_shift < FIELD_SIZE; ++i) {
    result.bytes[i] = (bytes[i + byte_shift] >> sub_shift) +
                      (bytes[i + 1 + byte_shift] << (8 - sub_shift));
  }
  result.bytes[FIELD_SIZE - 1 - byte_shift] =
      bytes[FIELD_SIZE - 1] >> sub_shift;

  return result;
}

FieldElement FieldElement::operator*(const FieldElement &other) const {
  FieldElement result{0};
  std::uint32_t carry{0};

  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    size_t j{0};

    carry >>= 8;
    for (; j <= i; ++j) {
      carry += (static_cast<std::uint32_t>(this->bytes[j]) *
                static_cast<std::uint32_t>(other.bytes[i - j]));
    }
    for (; j < FIELD_SIZE; ++j) {
      carry +=
          (static_cast<std::uint32_t>(this->bytes[j]) *
           static_cast<std::uint32_t>(other.bytes[i + FIELD_SIZE - j]) * 38);
    }

    result.bytes[i] = carry;
  }

  result.bytes[FIELD_SIZE - 1] &= 0b01111111;
  carry = (carry >> 7) * 19;
  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    carry += result.bytes[i];
    result.bytes[i] = carry;
    carry >>= 8;
  }

  return result.normalize();
}

FieldElement FieldElement::pow(const FieldElement &exponent) const {
  // Caution: this is vulnerable to a side-channel attack
  // since the computation time depends on the exponent
  if (exponent == FieldElement(0)) {
    return FieldElement(1);
  }

  FieldElement result{this->pow(exponent >> 1)};
  result = result * result;

  if (exponent.bit_at(0)) {
    result = result * (*this);
  }

  return result;
}

FieldElement FieldElement::inverse() const {
  if (*this == FieldElement{0}) {
    throw std::runtime_error("Division by zero FieldElement");
  }
  // By Fermat, x^(p-2) is a multiplicative inverse of x mod p
  // So we raise to the power of 2^255 - 19 - 2
  // which is 111...11101011
  // we start at the least significant digit

  FieldElement exponent{(FieldElement(1) << 255) - FieldElement(21)};
  return this->pow(exponent);
}

FieldElement FieldElement::mask_most_significant_bit() const {
  FieldElement result{*this};

  result.bytes[FIELD_SIZE - 1] &= 0b01111111;

  return result;
}

FieldElement FieldElement::prepare_scalar() const {
  // bit masks according to page 7 of RFC 7748
  FieldElement result{this->mask_most_significant_bit()};

  result.bytes[FIELD_SIZE - 1] |= 0b01000000;
  result.bytes[0] &= 0b11111000;

  return result;
}

bool FieldElement::is_zero_constant_time() const {
  bool have_seen_a_set_bit{false};
  for (size_t i{0}; i < FIELD_SIZE; ++i) {
    have_seen_a_set_bit |= this->bit_at(i);
  }

  return !have_seen_a_set_bit;
}
