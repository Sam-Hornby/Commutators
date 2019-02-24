#include "Operator.hpp"
#include "Expression.hpp"

namespace operators {

Expression Operator::operator+(const Operator & A);
  Expression operator+(const Operator & A) const {
    Expression exp;
    exp.expression.resize(2);
    exp.expression[0].push_back(*this);
    exp.expression[1].push_back(A);
    return exp;
  }


} // end namespace
