#pragma once

#include <string>

namespace operators {

struct NamedNumber {
  char symbol;
  bool complex_conjugated = false;
  explicit NamedNumber(char symbol) : symbol(symbol) {}
  explicit NamedNumber(char symbol, bool conj) : symbol(symbol), complex_conjugated(conj) {}
  std::string name() const {
    std::string result(1, symbol);
    if (complex_conjugated) {
      result += "!";
    }
    return result;
  }
  bool operator==(const NamedNumber other) const {
    return std::tie(symbol, complex_conjugated) ==
           std::tie(other.symbol, other.complex_conjugated);
  }
  bool operator!=(const NamedNumber other) const {
    return not (*this == other);
  }
  bool operator<(const NamedNumber other) const {
    return std::tie(symbol, complex_conjugated) <
           std::tie(other.symbol, other.complex_conjugated);
  }
};

}
