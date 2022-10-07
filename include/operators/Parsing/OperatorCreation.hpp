#include <Expression/Expression.hpp>
#include <string_view>
#include <string>
#include <vector>
#include <Expression/Infos/Fock1D.hpp>
#include <Expression/Infos/GenericInfo.hpp>
#include <absl/algorithm/algorithm.h>

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
  Info::unimplemented();
}

bool valid_axis(char c) {
  return c == 'x' or c == 'y' or c == 'z';
}

template <class Exception>
inline void error_assert(bool to_check, Exception exception) {
  if (!to_check) {
    throw exception;
  }
}

Operator<Fock1DInfo> handle_spin_operator(const std::string_view & token) {
  error_assert(valid_axis(token[1]),
                std::logic_error("Invalid axis for spin operator"));
  error_assert((token.size() >= 4 and token[2] == '_'),
                std::logic_error("Invalid format"));
  return Operator<Fock1DInfo>(
         ordering_value(0),
         Fock1DInfo(std::stoi(std::string(token.substr(3))), token[1],
                    Type::SPIN_OPERATORS));
}

std::pair<int, int> collect_vector_info(const std::string_view & str) {
  error_assert(absl::c_count(str, ',') == 1,
                std::logic_error("Too many commas"));
  auto it = absl::c_find(str, ',');
  auto index = std::distance(str.begin(), it);
  return std::make_pair(
        std::stoi(std::string(str.substr(0, index))),
        std::stoi(std::string(str.substr(index + 1)))
  );
}

Operator<Fock1DInfo> handle_bra(const std::string_view & token) {
  error_assert(token[token.size() - 1] == '>',
                std::logic_error("Invalid format"));
  if (token.size() == 3 and token[1] == '0') {
    return Operator<Fock1DInfo>(
              ordering_value(0),
              Fock1DInfo(0, Type::STATE_VECTOR));
  }
  // find comma then stoi both
  auto [coordinate, state] = collect_vector_info(token.substr(1, token.size() - 2));
  return Operator<Fock1DInfo>(
            ordering_value(0),
            Fock1DInfo(coordinate, state, Type::STATE_VECTOR));
}

Operator<Fock1DInfo> handle_ket(const std::string_view & token) {
  error_assert(token[token.size() - 1] == '|',
                std::logic_error("Invalid format"));
  if (token.size() == 3 and token[1] == '0') {
    return Operator<Fock1DInfo>(
              ordering_value(0),
              Fock1DInfo(0, Type::HC_STATE_VECTOR));
  }
  auto [coordinate, state] = collect_vector_info(token.substr(1, token.size() - 2));
  return Operator<Fock1DInfo>(
          ordering_value(0),
          Fock1DInfo(coordinate, state, Type::HC_STATE_VECTOR));
}

Operator<Fock1DInfo> handle_op(const std::string_view & token) {
  if (token.size() == 1) {
    return Operator<Fock1DInfo>(ordering_value(0),
                                Fock1DInfo(0, token[0], Type::ANIHILATION_OPERATOR));
  }
  error_assert(token.size() > 2 and token[1] == '_',
                std::logic_error(std::string("Invalid Format: ") + std::string(token)));
  return Operator<Fock1DInfo>(
        ordering_value(0),
        Fock1DInfo(std::stoi(std::string(token.substr(2))),
                    token[0], Type::ANIHILATION_OPERATOR));
}

template <> Operator<Fock1DInfo>
create_operator_from_string(std::string_view token) {
  spdlog::trace("Creating OP from token[{}]", token);
  if (token.size() == 0) {
    throw std::logic_error("Empty token");
  }
  switch (token[0]) {
    case 'S': {
      return handle_spin_operator(token);
    }
    case '|': {
      return handle_bra(token);
    }
    case '<': {
      return handle_ket(token);
    }
    default: {
      return handle_op(token); 
    }
  }
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