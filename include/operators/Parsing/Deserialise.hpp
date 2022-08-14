#pragma once 

#include <Expression/Expression.hpp>
#include <absl/algorithm/algorithm.h>
#include <absl/algorithm/container.h>
#include <string_view>
#include <vector>
#include <absl/strings/str_join.h>
#include <cctype>
#include "ParsingTree.hpp"
#include "OperatorCreation.hpp"
#include <Utils/PrintUtil.hpp>
#include <Utils/Trace.hpp>

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

struct OpInfo {
  int precidence;
  int num_operands;
  bool left_bind;
};

// TODO deal with unary minus
struct ParsingInfo {
  const std::string brackets{"()[]{}"};
  const std::string special_chars = brackets + "+*-/!#";
  const std::string unary_minus{"([{+*-/"};
  const std::unordered_map<std::string_view, OpInfo> op_infos = {
    {"+", {10, 2, true}},
    {"-", {10, 2, true}},
    {"*", {20, 2, true}},
    {"/", {20, 2, true}},
    {"#", {25, 1, false}}, // unary minus
    {"**", {30, 2, false}},
    {"!", {40, 1, true}},
    {"root", {40, 1, false}},
    // brackets treated seperately
  };

  std::optional<OpInfo> get_op_info(std::string_view token) const {
    auto it = op_infos.find(token);
    if (it == op_infos.end()) {
      return {};
    }
    return it->second;
  }

  static bool any_of_helper(const std::string & set, char c) {
    return absl::c_any_of(set, [&] (char s) {
      return s == c;
    });
  }

  bool is_bracket(std::string_view token) const {
    return any_of_helper(brackets, token[0]);
  }
  bool token_end(char c) const {
    return any_of_helper(special_chars, c);
  }
  bool can_affect_minus(char c) const {
    return any_of_helper(unary_minus, c);
  }
};

const ParsingInfo parsing_info;

bool brackets_are_valid(const std::string & input) {
  std::vector<char> expected;
  for (auto c : input) {
    if (c == '(') {
        expected.push_back(')');
    } else if (c == '{') {
        expected.push_back('}');
    } else if (c == '[') {
        expected.push_back(']');
    } else if (c == ')' or c == ']' or c == '}') {
        if (expected.empty()) {
            return false;
        }
        if (expected.back() != c) {
            return false;
        }
        expected.pop_back();
    } else {
      continue; // don't care about any other char
    }
  }
  return expected.empty();
}

void check_input(const std::string & input) {
  if (!brackets_are_valid(input)) {
    throw std::logic_error("Mismatched brackets");
  }
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

void find_unary_minus(std::vector<std::string_view> & tokens) {
  if (tokens.size() and tokens[0] == "-") {
    tokens[0] = std::string_view("#");;
  }
  for (std::size_t i = 1; i < tokens.size(); ++i) {
    if (tokens[i] == "-") {
      const auto & prev = tokens[i-1];
      if (prev.size() == 1 and parsing_info.can_affect_minus(prev[0])) {
        // convert to unary minus
        tokens[i] = std::string_view("#");
      }
    }
  }
}

void remove_empty_brackets(std::vector<std::string_view> & tokens) {
  bool made_change = true;
  while (made_change) {
    made_change = false;
    for (std::size_t i = 1; i < tokens.size(); ++i) {
      if ((tokens[i-1] == "(" and tokens[i] == ")") or
          (tokens[i-1] == "[" and tokens[i] == "]") or
          (tokens[i-1] == "{" and tokens[i] == "}")) {
        tokens[i-1] = std::string_view();
        tokens[i] = std::string_view();
        made_change = true;
      }
    }
    remove_whitespace(tokens);
  }
}

void verify_conj(const std::vector<std::string_view> & tokens) {
  if (tokens[0] == "!") {
    throw std::logic_error("Can't conjugate nothing");
  }
  for (int i = 1; i < tokens.size(); ++i) {
    if (tokens[i] != "!") {
      continue;
    }
    if (tokens[i-1] == ")" or tokens[i-1] == "]" or
        tokens[i-1] == "}") {
      continue;
    }
    if (parsing_info.get_op_info(tokens[i-1]) or
        parsing_info.is_bracket(tokens[i-1])) {
      throw std::logic_error(
          "Conjugate operator cannot follow " +
              std::string(tokens[i-1]));
    }
  }
}

struct TokenAndInfo {
  std::string_view token;
  OpInfo info;
  std::string to_string() const {
    return std::string(token);
  }
};

template<class Info>
struct SYAContext {
  int named_number = 0;
  int composite_number = 0;
  std::vector<TokenAndInfo> operator_stack;
  std::vector<std::unique_ptr<TreeNodeBase>> operand_stack;

  std::string to_string() const {
    return absl::StrCat("SYAContext: ",
                        named_number, ", ",
                        composite_number, ", [",
                        absl::StrJoin(operator_stack, ",", ToStringFormatter{}), "], ",
                        operand_stack.size(), ", {",
                        absl::StrJoin(operand_stack, ",", ApplyFormatter([] (const auto & n) {
                          return print_tree<Expression<Info>>(*n);
                        })), "}"
                      );
  }
};

template <class Info>
void apply_to_operands(SYAContext<Info> & context,
        std::string_view token, OpInfo info) {
  auto & operand_stack = context.operand_stack;
  if (operand_stack.size() < info.num_operands) {
    throw std::logic_error("Syntax err, operand stack too small");
  }
  std::vector<std::unique_ptr<TreeNodeBase>> args;
  std::size_t begin = operand_stack.size() - info.num_operands;
  for (std::size_t i = begin; i < operand_stack.size(); ++i) {
    args.emplace_back(std::move(operand_stack[i]));
  }
  operand_stack.erase(operand_stack.begin() + begin, operand_stack.end());
  operand_stack.emplace_back(create_function_node(token, std::move(args)));
}

template <class Info>
bool can_push(const SYAContext<Info> & context, std::string_view token, OpInfo info) {
  return context.operator_stack.empty() or
         context.operator_stack.back().token == "(" or
         (info.precidence > context.operator_stack.back().info.precidence) or
         (info.precidence == context.operator_stack.back().info.precidence and !info.left_bind);
}

template <class Info>
void SYAHandleOp(
      SYAContext<Info> & context, std::string_view token,
      OpInfo info) {
  if (context.named_number) {
    throw std::logic_error("Can't have operators inside named number");
  }
  if (info.num_operands == 1 and info.left_bind) {
    // special case and just apply imediately
    // not clear what happens with sqrt(a)!
    apply_to_operands(context, token, info);
    return;
  }

  while(!can_push(context, token, info)) {
    auto [t, op_info] = context.operator_stack.back();
    apply_to_operands(context, t, op_info);
    context.operator_stack.pop_back();
  }
  context.operator_stack.emplace_back(TokenAndInfo{token, info});
}

template <class Info>
void handle_operand(SYAContext<Info> & context, std::string_view token) {
  Expression<Info> result;
  if (context.named_number) {
    result = create_named_number_from_string<Info>(token);
  } else if (context.composite_number) {
    std::abort(); // don't know yet
  } else if (token[0] >= '0' and token[0] <= '9') {
    // assume is digit
    result = {{{number<Info>(std::stod(std::string(token)))}}};
  } else {
    result = {{{create_operator_from_string<Info>(token)}}};
  }
  context.operand_stack.emplace_back(create_variable_node(std::move(result)));
}

template <class Info>
void flush_operator_stack(SYAContext<Info> & context) {
  while (context.operator_stack.size()) {
    auto [token, info] = context.operator_stack.back();
    apply_to_operands(context, token, info);
    context.operator_stack.pop_back();
  }
}

bool is_open_bracket(const std::string_view token) {
  return token[0] == '(' or token[0] == '[' or token[0] == '{';
}

template <class Info>
void handle_bracket(SYAContext<Info> & context, const std::string_view token) {
  if (is_open_bracket(token)) {
    context.operator_stack.emplace_back(TokenAndInfo{token, {}});
  } else {
    while(!is_open_bracket(context.operator_stack.back().token)) {
      auto [t, info] = context.operator_stack.back();
      apply_to_operands(context, t, info);
      context.operator_stack.pop_back();
    }
    context.operator_stack.pop_back();
  }
  if (token == "[") {
    if (context.named_number > 0) {
      throw std::logic_error("Can't nest named numbers");
    }
    context.named_number++;
  } else if (token == "]") {
    context.named_number--;
  } else if (token == "{") {
    if (context.named_number > 0) {
      throw std::logic_error("Can't nest named numbers");
    }
  }
}

// Shunting yard algorithm to transform expression to postfix version
template <class Info>
std::unique_ptr<TreeNodeBase>
shunting_yard(const std::vector<std::string_view> & tokens) {
  if (tokens.empty()) {
    return create_variable_node(Expression<Info>());
  }

  SYAContext<Info> context;
  for (const auto & token : tokens) {
    spdlog::debug("SYA next: {}, context: {}", token, context.to_string());
    if (parsing_info.is_bracket(token)) {
      // handle bracket
      handle_bracket<Info>(context, token);
    } else if (parsing_info.get_op_info(token)) {
      SYAHandleOp(context, token, *parsing_info.get_op_info(token));
    } else {
      handle_operand<Info>(context, token);
    }
  }
  
  // need to move function operators into
  flush_operator_stack(context);

  if (context.operand_stack.size() != 1) {
    throw std::logic_error("Syntax error");
  }
  return std::move(context.operand_stack[0]);

}

// For now just support *,+,(,) and forget about root, square etc
template <class Info>
Expression<Info> from_string(std::string input) {
  TRACE(check_input(input));
  auto tokens = TRACE(tokenise_exp(input));
  TRACE(find_unary_minus(tokens));
  TRACE(remove_empty_brackets(tokens));
  TRACE(verify_conj(tokens));
  spdlog::debug("Tokens: {}", absl::StrJoin(tokens, ","));
  auto ast = TRACE(shunting_yard<Info>(tokens));
  spdlog::debug("AST: {}", print_tree<Expression<Info>>(*ast));
  return TRACE(create_expression<Info>(*ast));
}


}