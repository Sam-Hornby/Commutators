#include <Expression/Expression.hpp>
#include <Expression/Operator.hpp>
#include <Comutators/GenericComutators.hpp>
#include <Numbers/CompositeNumberExpressions.hpp>
#include <tuple>
#define BOOST_TEST_MODULE CompositeNumbers
#include <boost/test/included/unit_test.hpp>
#include <spdlog/fmt/fmt.h>



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
  auto e = ExpressionExpr::create(convertNumbersToEmptyInfo<GenericInfo>(exp1));

  auto f = AddExpr::create(ComplexNumberExpr::create(ComplexNumber(2.0, 0)),
                           ComplexNumberExpr::create(ComplexNumber(2.0, 0)));

  auto g = SquareExpr::create(f);
  auto h = MulExpr::create(a_, SquareRootExpr::create(f));

  BOOST_CHECK_EQUAL(exp1.print(false), fmt::format("({}) + ({})", a_.name(), b.name()));
  BOOST_CHECK_EQUAL(a_.name(), "a");
  BOOST_CHECK_EQUAL(b_.name(), ComplexNumber(1.0, 2.0).name());
  BOOST_CHECK_EQUAL(c_.name(), fmt::format("\u221A{}", b_.name()));
  BOOST_CHECK_EQUAL(d.name(), fmt::format("({}/{})", a_.name(), c_.name()));
  BOOST_CHECK_EQUAL(e.name(), exp1.name());
  BOOST_CHECK_EQUAL(g.simplify().name(), "16.000000");
  BOOST_CHECK_EQUAL(h.simplify().name(), "(a*2.000000)");
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

