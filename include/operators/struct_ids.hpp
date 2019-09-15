#ifndef _struct_ids_hpp_
#define _struct_ids_hpp_

namespace operators {

enum class Type {
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

struct Fock1DInfo {
  int x_coordinate;
  int state = 0;   // This field is only used for state not creation/anihilation ops
  Type type = Type::UNSPECIFIED;

  Fock1DInfo(int x_coordinate) : x_coordinate(x_coordinate) {}
  Fock1DInfo(int x_coordinate, Type type) : x_coordinate(x_coordinate), type(type) {}
  Fock1DInfo(int x_coordinate, int state, Type type) : x_coordinate(x_coordinate), state(state), type(type) {}
  Fock1DInfo() = default;

  bool operator==(Fock1DInfo other) const {
    return x_coordinate == other.x_coordinate;
  }

  bool operator!=(Fock1DInfo other) const {
    return x_coordinate != other.x_coordinate;
  }
  static Fock1DInfo vacuumState() {
    return Fock1DInfo(0, 0, Type::STATE_VECTOR);
  }
  bool isVacuumState() {
    return type == Type::STATE_VECTOR and state == 0;
  }
  bool isHCVacuumState() {
    return type == Type::HC_STATE_VECTOR and state == 0;
  }
  bool match(const Fock1DInfo & other) {
    if (isFockOpType(type) and isFockOpType(other.type)) {
      return x_coordinate == other.x_coordinate;
    }
    if (isVectorType(type) and isVectorType(other.type)) {
      return state == other.state and x_coordinate == other.x_coordinate;
    }
    return false;
  }
};

struct ordering_value {
  int value;

  ordering_value(int value) : value(value) {}
  ordering_value() = default;

  bool operator<(ordering_value other) const {
    return value < other.value;
  }

  bool operator>(ordering_value other) const {
    return value > other.value;
  }

  bool operator==(ordering_value other) const {
    return value == other.value;
  }

  bool operator!=(ordering_value other) const {
    return value != other.value;
  }
};

// TODO addition_id and multiply_id

} // end namespace operators

#endif
