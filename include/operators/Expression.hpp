#ifndef _expression_hpp_
#define _expression_hpp_

#include <vector>
#include <iostream>
#include "Operator.hpp"


namespace operators {

// [A, B] = AB - BA
// BA = AB - [A, B]
// AB = [A, B] + BA

class Expression {
  Expression sort_multiply_term(const std::vector<Operator> &term,
                                std::function<Expression(const Operator &, const Operator &)> commute) const;
public:
  std::vector<std::vector<Operator>> expression;  // terms in inner vectors are considered multiplied, outer added
  void print(std::ostream & out = std::cout) const;   // print the expression
  Expression sort(std::function<Expression(const Operator &, const Operator &)> commute) const;
  Expression simplify_numbers() const;
  Expression(std::vector<std::vector<Operator>> expression) : expression(expression) {};
  Expression() = default;

};

Expression operator+(const Operator & A, const Operator & B);
Expression operator*(const Operator & A, const Operator & B);

Expression operator+(const Expression & A, const Operator & B);
Expression operator*(const Expression & A, const Operator & B);

Expression operator+(const Expression & A, const Expression & B);
Expression operator*(const Expression & A, const Expression & B);

} // end namespace operators

#endif // header guard
