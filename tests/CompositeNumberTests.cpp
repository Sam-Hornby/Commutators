#include <Expression/Expression.hpp>
#include <Expression/Operator.hpp>
#include <Comutators/GenericComutators.hpp>
#include <Numbers/CompositeNumberExpressions.hpp>
#include <tuple>
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

BOOST_AUTO_TEST_CASE(Operators) {
  auto a = NamedNumberExpr::create(NamedNumber('a'));
  auto b = NamedNumberExpr::create(NamedNumber('b'));
  auto c = ComplexNumberExpr::create(ComplexNumber(1.0, 2.0));
  auto d = ComplexNumberExpr::create(ComplexNumber(3.0, 2.0)); 
  auto e = SquareRootExpr::create(a);
  auto f = SquareRootExpr::create(b);
  auto g = SubExpr::create(a, b);
  auto h = SubExpr::create(b, a);
 
  // check equality of different classes always fails
  BOOST_CHECK(a != c);
  BOOST_CHECK(c != a);
  BOOST_CHECK(a != e);
  BOOST_CHECK(e != a);
  BOOST_CHECK(a != g);
  BOOST_CHECK(g != a);

  // check operators when comparing same classes
  BOOST_CHECK(a == a);
  BOOST_CHECK(a != b);
  BOOST_CHECK(!(a < a));
  BOOST_CHECK((a < b) == ('a' < 'b'));
  
  
  BOOST_CHECK(c == c);
  BOOST_CHECK(c != d);
  BOOST_CHECK(!(c < c));
  BOOST_CHECK((c < d) == (ComplexNumber(1.0, 2.0) < ComplexNumber(3.0, 2.0)));
  

  BOOST_CHECK(e == e);
  BOOST_CHECK(e != f);
  BOOST_CHECK(!(e < e));
  BOOST_CHECK((e < f) == (a < b));

  BOOST_CHECK(g == g);
  BOOST_CHECK(g != h);
  BOOST_CHECK(!(g < g));
  BOOST_CHECK((g < h) == (std::tie(a, b) < std::tie(b, a)));
}

