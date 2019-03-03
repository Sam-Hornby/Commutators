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

struct operator_info {
  int value;
  Type type = Type::UNSPECIFIED;

  operator_info(int value) : value(value) {}
  operator_info(int value, Type type) : value(value), type(type) {}
  operator_info() = default;

  bool operator==(operator_info other) const {
    return value == other.value;
  }

  bool operator!=(operator_info other) const {
    return value != other.value;
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
