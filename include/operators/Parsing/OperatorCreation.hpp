#include <Expression/Expression.hpp>
#include <string_view>
#include <vector>
#include <Expression/Infos/Fock1D.hpp>
#include <Expression/Infos/GenericInfo.hpp>

namespace operators {

template <class Info>
Expression<Info>
create_named_number_from_string(std::string_view token) {
  if (token.size() != 1) {
    throw std::logic_error("Named number can only be single character");
  }
  return {{{named_number<Info>(token[0])}}};
}

template <class Info>
Operator<Info>
create_operator_from_string(std::string_view token) {
  std::abort();
}

bool valid_axis(char c) {
  return c == 'x' or c == 'y' or c == 'z';
}

template <> Operator<Fock0DInfo>
create_operator_from_string(std::string_view token) {
  if (token.size() == 1) {
    return Operator<Fock0DInfo>(ordering_value(0),
                Fock0DInfo(Type::ANIHILATION_OPERATOR, token[0]));
  }
  if (token.size() == 2) {
    if (token[0] != 'S') {
      throw std::logic_error("Only single char or char_0 formats allowed");
    }
    if (!valid_axis(token[1])) {
      throw std::logic_error("Invalid axis for spin operator");
    }
    return Operator<Fock0DInfo>(ordering_value(0),
          Fock0DInfo(Type::SPIN_OPERATORS, token[1]));
  }
  if (token.size() != 3) {
    throw std::logic_error("Invalid input for operator");
  }
  if (token[1] == '_' and token[2] == '0') {
    return Operator<Fock0DInfo>(ordering_value(0),
                    Fock0DInfo(Type::ANIHILATION_OPERATOR, token[0]));
  }
  if (token == "|0>") {
    return Operator<Fock0DInfo>(ordering_value(0),
                    Fock0DInfo(Type::STATE_VECTOR));
  }
  if (token == "<0|") {
    return Operator<Fock0DInfo>(ordering_value(0),
                    Fock0DInfo(Type::HC_STATE_VECTOR));
  }
  throw std::logic_error("Invalid input for operator");
}

}