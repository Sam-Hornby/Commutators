#pragma once 

#include <Expression/Expression.hpp>
#include <absl/algorithm/algorithm.h>
#include <absl/algorithm/container.h>
#include <string_view>
#include <vector>
#include <absl/strings/str_join.h>
#include <cctype>

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


struct ParsingInfo {
  const std::string special_chars{"+*-/!()[]{}"};

  bool token_end(char c) const {
    return absl::c_any_of(special_chars, [&] (char s) {
      return s == c;
    });
  }
};

const ParsingInfo parsing_info;

void check_input(const std::string & input) {
  // check brackets
}

bool end_of_current_token(std::string::const_iterator it) {
  return std::isspace(*it) or parsing_info.token_end(*it);
}

template <class It>
std::string_view create_view(It begin, It end) {
  return std::string_view(&(*begin), std::distance(begin, end));
}

void remove_whitespace(std::vector<std::string_view> & result) {
  auto last = std::remove_if(result.begin(), result.end(), [] (std::string_view tok) {
    return tok.size() == 0 or std::isspace(tok[0]);
  });
  result.erase(last, result.end());
}

void find_pow_operators(std::vector<std::string_view> & result) {
  for (std::size_t i = 1; i < result.size(); ++i) {
    if (result[i-1] == "*" and result[i] == "*") {
      result[i-1] = create_view(result[i-1].begin(),
                                result[i].end());
      result[i] = std::string_view();
    }
  }
  remove_whitespace(result);
}

std::vector<std::string_view> tokenise_exp(const std::string & input) {
  std::vector<std::string_view> result;

  auto token_begin = input.begin();
  for (auto it = input.begin(); it < input.end();) {
    // commit last token to result
    if (end_of_current_token(it)) {
      result.emplace_back(create_view(token_begin, it));
    } else {
      ++it;
      continue;
    }
    // put all the special chars into single tokens
    while (it < input.end() and end_of_current_token(it)) {
      // all specials are single characters so add them and increment
      result.emplace_back(create_view(it, it + 1));
      ++it;
    }
    token_begin = it;
  }
  result.emplace_back(create_view(token_begin, input.end()));
  remove_whitespace(result);
  find_pow_operators(result);
  return result;
}

// For now just support *,+,(,) and forget about root, square etc
template <class OperatorInfo>
Expression<OperatorInfo> from_string(std::string input) {
  check_input(input);
  auto tokens = tokenise_exp(input);
  spdlog::debug("Tokens : {}", absl::StrJoin(tokens, ","));
  // Shunting yard algorithm to transform expression to postfix version
  // rules are:
  //   - operands are pushed to queue instantly
  //   - ( is pushed onto stack
  //   - ) process all symbols on stack until hit ( then discard
  //   - 
  return {};
}


}