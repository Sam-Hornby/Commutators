#ifndef _struct_ids_hpp_
#define _struct_ids_hpp_

namespace operators {

struct operator_id {
  int value;

  operator_id(int value) : value(value) {}
  operator_id() = default;
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
};

// TODO addition_id and multiply_id

} // end namespace operators

#endif
