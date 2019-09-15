#ifndef _operator_hpp_
#define _operator_hpp_

#include <boost/optional.hpp>
#include <string>
#include "struct_ids.hpp"



namespace operators {

template <class OperatorInfo>
struct Operator {
  std::string name;  // name of operator TODO make const
  ordering_value order;  // when sorting this determines greater than or equal to
  OperatorInfo info;     // should be unique for each operator TODO make const
  // if this is set operator is a number, comutators are automatically assumed zero, op_id is irelevant
  boost::optional<double> value;

  Operator() = default;
  Operator(std::string name, ordering_value order, OperatorInfo info) : name(name), order(order), info(info) {}
  Operator(double v) : order(ordering_value(std::numeric_limits<int>::min())) {
    value = v;
    name = std::to_string(v);
  }

  bool is_number() const {
    return static_cast<bool>(value);
  }

  bool operator<(Operator other) const {
    return order < other.order;
  }

  bool operator>(Operator other) const {
    return order > other.order;
  }

  bool operator==(Operator other) const {
    return (name == other.name) and (order == other.order) and (info == other.info);
  }

  bool operator!=(Operator other) const {
    return !(*this == other);
  }

};

template <class OperatorInfo>
Operator<OperatorInfo> number(const double n);

//******************************************************************
// All definitions below
//******************************************************************

template <class OperatorInfo>
Operator<OperatorInfo> number(const double n) {
  return Operator<OperatorInfo>(n);
}

} // end namespace

#endif // header guard
