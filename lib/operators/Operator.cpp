//#include "Operator.hpp"

#include "Expression.hpp"
#include "Operator.hpp"

namespace operators {

Expression operator+(const Operator & A, const Operator & B) {
  Expression exp;
  exp.expression.resize(2);
  exp.expression[0].push_back(A);
  exp.expression[1].push_back(B);
  return exp;
}

Expression operator*(const Operator & A, const Operator & B) {
  Expression exp;
  std::vector<Operator> m;
  m.push_back(A);
  m.push_back(B);
  exp.expression.push_back(std::move(m));
  return exp;
}

Operator number(const int n) {
  return Operator(n);
}


} // end namespace
