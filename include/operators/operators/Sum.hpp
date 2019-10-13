#pragma once

#include "../Expression.hpp"
#include "iostream"

namespace operators {

// A struct only really used for printing
template <class OperatorInfo>
struct Sum {
  Expression<OperatorInfo> exp;
  Sum() = default;
  Sum(Expression<OperatorInfo> exp) : exp(std::move(exp)) {}

  std::string print() const {
    return "\u03A3[" + exp.print(false) + "]";
  }
  void print_out() {
    std::cout << this->print();
  }
};

template <class OperatorInfo>
struct Product {
  Expression<OperatorInfo> exp;
  Product() = default;
  Product(Expression<OperatorInfo> exp) : exp(std::move(exp)) {}

  std::string print() const {
    return "\u03A0[" + exp.print(false) + "]";
  }
  void print_out() {
    std::cout << this->print();
  }
};


}