#ifndef _operator_hpp_
#define _operator_hpp_

#include <boost/optional.hpp>
#include <string>
#include "struct_ids.hpp"


namespace operators {

struct Operator {
  std::string name;  // name of operator TODO make const
  ordering_value order;  // when sorting this determines greater than or equal to
  operator_info info;     // should be unique for each operator TODO make const
  // if this is set operator is a number, comutators are automatically assumed zero, op_id is irelevant
  boost::optional<int> value;

  Operator() = default;
  Operator(std::string name, ordering_value order, operator_info info) : name(name), order(order), info(info) {}
  Operator(int v) : info(operator_info(777777)), order(ordering_value(-7777)) {
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

  //Expression operator+(const Operator & A) const;

};

} // end namespace

#endif // header guard
