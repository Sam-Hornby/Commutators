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

struct Fock1DInfo {
  int x_coordinate;
  Type type = Type::UNSPECIFIED;

  Fock1DInfo(int x_coordinate) : x_coordinate(x_coordinate) {}
  Fock1DInfo(int x_coordinate, Type type) : x_coordinate(x_coordinate), type(type) {}
  Fock1DInfo() = default;

  bool operator==(Fock1DInfo other) const {
    return x_coordinate == other.x_coordinate;
  }

  bool operator!=(Fock1DInfo other) const {
    return x_coordinate != other.x_coordinate;
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
