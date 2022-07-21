#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <absl/types/span.h>
#include <absl/strings/str_cat.h>
#include <Utils/Utils.hpp>

namespace operators {

// -----------------------------------------------------------------------------
// Tree structs
// -----------------------------------------------------------------------------

struct TreeNodeBase {
  virtual ~TreeNodeBase() = default;
};

template <class T>
struct VariableNode : TreeNodeBase {
  T variable;
  VariableNode(T var) : variable(std::move(var)) {}
};

struct FunctionNode : TreeNodeBase {
  std::string name;
  std::vector<std::unique_ptr<TreeNodeBase>> args;
  FunctionNode(std::string name, std::vector<std::unique_ptr<TreeNodeBase>> args) :
        name(std::move(name)), args(std::move(args)) {}
};

// -------------------------------------------------------------------------------
// Creation functions
// -------------------------------------------------------------------------------

template <class T>
std::unique_ptr<TreeNodeBase> create_variable_node(T var) {
  return std::make_unique<VariableNode<T>>(std::move(var));
}

std::unique_ptr<TreeNodeBase>
create_function_node(std::string name, std::vector<std::unique_ptr<TreeNodeBase>> && args) {
  return std::make_unique<FunctionNode>(std::move(name), std::move(args));
}


const std::unordered_set<std::string_view> supported_ops {
  "+",
  "-",
  "*",
  "/",
  "!",
  "**",
  "exp",
  "\u221A", // root
};

bool is_digit(std::string_view token) {
  return (token[0] >= '0' and token[0] <= '9') or token[0] == 'i';
}

template <class T>
std::unique_ptr<TreeNodeBase> create_var_node_from_token(std::string_view token) {
  if (supported_ops.count(token)) {
    throw std::logic_error("Trying to create variable from function token");
  }
  std::abort();
  // fist check if it is a digit, if so create a number
  /*if (is_digit(token)) {
    // create complex number
    return;
  }
  if (token[0] == '[') {
    // create named number
    return;
  }
  if (token[0] == '{') {
    // create composite number
    return;
  }
  // last option left if operator
  return;*/
}

// -------------------------------------------------------------------------------
// Iteration utils
// -------------------------------------------------------------------------------

template <class Visitor, class VarType, class RetType>
RetType apply_visitor(const TreeNodeBase & node, Visitor visitor = Visitor()) {
  const FunctionNode * fnode =
        dynamic_cast<const FunctionNode *>(&node);
  if (fnode) {
    return visitor.handle_function(*fnode);
  }
  const VariableNode<VarType> * vnode =
          dynamic_cast<const VariableNode<VarType> *>(&node);
  if (vnode) {
    return visitor.handle_variable(*vnode);
  }
  std::abort();
}

using ChildrenView = absl::Span<const std::unique_ptr<TreeNodeBase>>;

ChildrenView get_children(const TreeNodeBase & node) {
  const FunctionNode * fnode = dynamic_cast<const FunctionNode *>(&node);
  if (!fnode) {
    return ChildrenView();
  }
  return ChildrenView(fnode->args);
} 

void no_op(const TreeNodeBase & node) {}

template <class Before, class After>
void dfs_walk(const TreeNodeBase & node, Before before, After after) {
  before(node);
  for (const auto & child : get_children(node)) {
    dfs_walk(*child, before, after);
  }
  after(node);
}

// -------------------------------------------------------------------------------
// Evaluation utils
// -------------------------------------------------------------------------------

template <class T>
struct EvaluateVisitor {
  std::vector<T> args;
  EvaluateVisitor(std::vector<T> args) : args(std::move(args)) {}
  T handle_function(const FunctionNode & node) {
    if (node.name == "+") {
      assert(args.size() == 2);
      return args[0] + args[1];
    } else if (node.name == "-") {
      assert(args.size() == 2);
      return args[0].__sub__(args[1]);
    } else if (node.name == "*") {
      assert(args.size() == 2);
      return args[0] * args[1];
    } else if (node.name == "/") {
      assert(args.size() == 2);
      std::abort();
      //return args[0] / args[1];
    } else if (node.name == "**") {
      assert(args.size() == 2);
      // Need to check that second arg evaluates to an int
      throw std::logic_error("Don't support ** yet");
    } else if (node.name == "!") {
      assert(args.size() == 1);
      return hermition_conjugate(args[0]);
    }
    // TODO list is:
    //   - root, exp, ^2
    throw std::logic_error(absl::StrCat("Don't support operator ", node.name));
  }
  T handle_variable(const VariableNode<T> & node) const {
    return node.variable;
  }
};

template <class T>
T create_expression(const TreeNodeBase & root) {
  std::vector<T> args;
  for (const auto & child : get_children(root)) {
    args.emplace_back(create_expression<T>(*child));
  }
  return apply_visitor<EvaluateVisitor<T>, T, T>(root, EvaluateVisitor<T>(std::move(args)));
}

// -------------------------------------------------------------------------------
// To string utils
// -------------------------------------------------------------------------------

template <class T>
struct NameVisitor {
  std::string handle_function(const FunctionNode & n) const {
    return n.name;
  }
  std::string handle_variable(const VariableNode<T> & n) const {
    return n.variable.name();
  }
}; 

template<class T>
std::string print_tree(const TreeNodeBase & node) {
  std::string result;
  dfs_walk(node,
           [&] (const TreeNodeBase &node) {
              result.append(apply_visitor<NameVisitor<T>, T, std::string>(node));
              result.push_back('(');
            },
            [&] (const TreeNodeBase & node) {
              result += "),";
            }
  );
  return result;
}



}