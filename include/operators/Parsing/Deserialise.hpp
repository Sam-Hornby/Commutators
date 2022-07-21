#pragma once 

#include <Expression/Expression.hpp>
#include <absl/algorithm/algorithm.h>
#include <string_view>
#include <vector>

namespace operators {
// Function to take a string a return an expression from it


// Plan
// - split into tokens
// - create an AST using shunting algo or recursion?
// - ops to handle: +-*/!(** for intergers only) and root
// - vars to handle:
//     - operator
//     - complex number (so must handle i)
//     - [named_number]
//     - {composite number}

template <class OperatorInfo>
struct OperandOrOp {
  Expression<OperatorInfo> operand;
  char op; 
};

void check_input(const std::string & input) {
  // check brackets
}

std::vector<std::string_view> tokenise_exp(const std::string & input) {
  return {};
}

// For now just support *,+,(,) and forget about root, square etc
template <class OperatorInfo>
Expression<OperatorInfo> from_string(std::string input) {
  check_input(input);
  auto tokens = tokenise_exp(input);

  // Shunting yard algorithm to transform expression to postfix version
  // rules are:
  //   - operands are pushed to queue instantly
  //   - ( is pushed onto stack
  //   - ) process all symbols on stack until hit ( then discard
  //   - 
  return {};
}


}