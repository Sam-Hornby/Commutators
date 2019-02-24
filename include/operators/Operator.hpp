#ifndef _operator_hpp_
#define _operator_hpp_

#include <boost/optional.hpp>
#include <string>
#include "struct_ids.hpp"
#include "Expression.hpp"


namespace operators {

class Expression;

struct Operator {
  std::string name;  // name of operator TODO make const
  ordering_value order;  // when sorting this determines greater than or equal to
  operator_id id;     // should be unique for each operator TODO make const
  // if this is set operator is a number, comutators are automatically assumed zero, op_id is irelevant
  boost::optional<int> value;

  Operator(std::string name, ordering_value order, operator_id id) : name(name), order(order), id(id) {}
  Operator(int v) : id(operator_id(777777)), order(ordering_value(-7777)) {
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

  Expression operator+(const Operator & A);

};

} // end namespace

#endif // header guard
