#include <Expression/Expression.hpp>
#include <Expression/Operator.hpp>
#include <Comutators/GenericComutators.hpp>
#include <Numbers/CompositeNumberExpressions.hpp>
#define BOOST_TEST_MODULE CompositeNumbers
#include <boost/test/included/unit_test.hpp>


using namespace operators;

BOOST_AUTO_TEST_CASE(Basic) {
  auto a_ = NamedNumberExpr::create(NamedNumber('a'));
  auto a = Operator<GenericInfo>(a_);
  auto b_ = ComplexNumberExpr::create(ComplexNumber(1.0, 2.0));
  auto b = Operator<GenericInfo>(b_);
  auto c_ = SquareRootExpr::create(b_);
  auto c = Operator<GenericInfo>(c_);
  auto d = Operator<GenericInfo>(DivExpr::create(a_, c_));

  auto exp1 = a + b;
  auto exp2 = a * b;
  auto exp3 = c + a;
  auto exp4 = d + a;

  exp1.print();
  exp2.print();
  exp3.print();
  exp4.print();

}
