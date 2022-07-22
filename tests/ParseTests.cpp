#include <Expression/Expression.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <Parsing/Deserialise.hpp>
#define BOOST_TEST_MODULE Parse
#include <boost/test/included/unit_test.hpp>


using namespace operators;



Operator<Fock0DInfo> creation_op(char c = 'a') {
  auto res = Operator<Fock0DInfo>(ordering_value(0),
                  Fock0DInfo(Type::CREATION_OPERATOR));
  res.info().symbol = c;
  return res;
}
Operator<Fock0DInfo> anihilation_op(char c = 'a') {
  auto res = Operator<Fock0DInfo>(ordering_value(0),
                  Fock0DInfo(Type::ANIHILATION_OPERATOR));
  res.info().symbol = c;
  return res;
}

struct InputAndResult {
  const char * input;
  Expression<Fock0DInfo> result;
};

const std::vector<InputAndResult> test_cases = {
  {"a_0", {{{anihilation_op()}}}},
  {"a_0!", {{{creation_op()}}}},
  {"c", {{{anihilation_op('c')}}}},
  {"c!", {{{creation_op('c')}}}},
  {"", {}},
  {"5", {{{number<Fock0DInfo>(5)}}}},
  {"-a", number<Fock0DInfo>(-1) * anihilation_op()},
  {"a!**3", creation_op() * creation_op() * creation_op()},
  {"a*b!+c_0*(d_0 + e!)",
      anihilation_op('a') * creation_op('b') + anihilation_op('c')
      * (anihilation_op('d') + creation_op('e'))},
  {"[a]", {{{named_number<Fock0DInfo>('a')}}}},
  //{Sz}, ...
  //{"{[a]}", ...}, 
  //{"{[a]! * 5}", ...},
  //{"{{b + c} * 5}", ...},
}; 

BOOST_AUTO_TEST_CASE(parse_successful) {
  for (const auto & test_case : test_cases) {
    auto result = from_string<Fock0DInfo>(test_case.input);
    BOOST_CHECK_EQUAL(result, test_case.result);
  }
}

const std::vector<std::string> fail_cases = {
  "abc_0", // to characters for op name
  "a!a", // no sensical operator
  "a b", // no operator between tokens
  "(a", // unmatched bracket
  "(a + [b)]", // invalid bracket sequence
  "a + + b", // invalid sequence
  "a *** 3", // invalid operator
  "[c * b]", // can't have op in named number
  "[[a]]", // can't have nested named numbers
  "[{a}]", // can't have composite inside named
  "[name]", // only single char names allowed
  "ab",
  "Sm",
};

BOOST_AUTO_TEST_CASE(parse_failed) {
  for (const auto & test : fail_cases) {
    BOOST_CHECK_THROW(from_string<Fock0DInfo>(test),
                      std::logic_error);
  }
}
