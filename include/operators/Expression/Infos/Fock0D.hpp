#pragma once 

namespace operators {


enum class Type : std::uint8_t{
  STATE_VECTOR,   // eg |0>
  HC_STATE_VECTOR,  // eg <0|
  CREATION_OPERATOR,  // fock space creation operator
  ANIHILATION_OPERATOR,  // fock space anihilation operator (hc of creation)
  UNSPECIFIED,  // all other operators
};

static bool isVectorType(const Type type) {
  return type == Type::STATE_VECTOR or type == Type::HC_STATE_VECTOR;
}
static bool isFockOpType(const Type type) {
  return type == Type::CREATION_OPERATOR or type == Type::ANIHILATION_OPERATOR;
}


struct Fock0DInfo {
  static Type conjugate_type(const Type type) { 
    switch (type) {
      case Type::STATE_VECTOR:
        return Type::HC_STATE_VECTOR;
      case Type::HC_STATE_VECTOR:
        return Type::STATE_VECTOR;
      case Type::CREATION_OPERATOR:
        return Type::ANIHILATION_OPERATOR;
      case Type::ANIHILATION_OPERATOR:
        return Type::CREATION_OPERATOR;
      case Type::UNSPECIFIED:
        return type;
    }
  }

  // struct where only degree of freedom is energy value and only vacuum state represented
  // other states representedwith fock operators
  Type type = Type::UNSPECIFIED;
  char symbol = 'a';
  Fock0DInfo() = default;
  Fock0DInfo(Type type) : type(type) {}
  std::string name() const {
    std::string result;
    if (isFockOpType(type)) {
      result += (std::to_string(symbol) + "_0");
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
  Type get_type() const {return type;}
  
  void complex_conjugate() {
    this->type = conjugate_type(this->type);
  }  

};

}
