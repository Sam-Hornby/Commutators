#pragma once 
#include "Fock1D.hpp"

namespace operators {


struct Fock0DInfo {
  // struct where only degree of freedom is energy value and only vacuum state represented
  // other states representedwith fock operators
  Type type = Type::UNSPECIFIED;
  Fock0DInfo() = default;
  Fock0DInfo(Type type) : type(type) {}
  std::string name() const {
    std::string result;
    if (isFockOpType(type)) {
      result += "a_0";
      if (type == Type::CREATION_OPERATOR) {
        result += "!";
      }
      return result;
    } else if (type == Type::STATE_VECTOR) {
      return "|0>";
    } else if (type == Type::HC_STATE_VECTOR) {
      return "<0|";
    } else {
      std::abort();
    }
  }
  bool operator==(Fock0DInfo other) const {
    return type == other.type;
  }

  bool operator!=(Fock0DInfo other) const {
    return not (*this == other);
  }
  bool operator<(Fock0DInfo other) const {
    return type < other.type;
  }
  static Fock0DInfo vacuumState() {
    return Fock0DInfo(Type::STATE_VECTOR);
  }
  bool isVacuumState() const {
    return type == Type::STATE_VECTOR;
  }
  bool isHCVacuumState() const {
    return type == Type::HC_STATE_VECTOR;
  }
  bool match(const Fock0DInfo & other) const {
    if (isFockOpType(type) and isFockOpType(other.type)) {
      return true;
    }
    if (isVectorType(type) and isVectorType(other.type)) {
      return true;
    }
    return false;
  }
};

}
