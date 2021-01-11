#pragma once

#include <vector>
#include <map>
#include <iostream>
#include "Operator.hpp"
#include <algorithm>
#include "Logging.hpp"
#include <sstream>
#include <boost/container/small_vector.hpp>


namespace operators {

enum class SortUsing {
  COMMUTATORS,     // the commute function passed to sort returns the commutator of the 2 operators
  ANTICOMMUTATORS, // the commute function passed to sort returns the anticommutator of the 2 operators
};

// [A, B] = AB - BA
// BA = AB - [A, B]
// AB = [A, B] + BA
// {A, B} = AB + BA
// AB = -BA + {A, B}

template <class T>
using vector_type = boost::container::small_vector<T, 1>;

template <class OperatorInfo>
class Expression {
public:
  vector_type<vector_type<Operator<OperatorInfo>>> expression;  // terms in inner vectors are considered multiplied, outer added
  void print(std::ostream & out = std::cout, const bool add_newline = true) const;   // print the expression
  std::string print(const bool add_newline) const;   // print the expression
  std::string name() const;
  Expression(vector_type<vector_type<Operator<OperatorInfo>>> expression) : expression(expression) {};
  Expression() = default;
};

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B);
template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B);

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Expression<OperatorInfo> & A, const Operator<OperatorInfo> & B);
template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Expression<OperatorInfo> & A, const Operator<OperatorInfo> & B);

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Operator<OperatorInfo> & A, const Expression<OperatorInfo> & B);
template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Operator<OperatorInfo> & A, const Expression<OperatorInfo> & B);

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B);
template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B);


template <class OperatorInfo>
bool operator==(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B);
template <class OperatorInfo>
bool operator!=(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B);
template <class OperatorInfo>
bool operator<(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B);


//******************************************************************
// All definitions below
//******************************************************************
//----------------------------------------------------------------------------------------------------------------------
// operator overloads
//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo>
std::ostream& operator<<(std::ostream& os, const Expression<OperatorInfo> &A) {
  os << A.print(false);
  return os;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  Expression<OperatorInfo> exp;
  exp.expression.resize(2);
  exp.expression[0] = {A};
  exp.expression[1] = {B};
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  Expression<OperatorInfo> exp;
  exp.expression.resize(1);
  exp.expression[0].resize(2);
  exp.expression[0][0] = A;
  exp.expression[0][1] = B;
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Expression<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  Expression<OperatorInfo> exp = A;
  vector_type<Operator<OperatorInfo>> m;
  m.push_back(B);
  exp.expression.push_back(std::move(m));
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Operator<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  Expression<OperatorInfo> exp({{A}});
  return exp + B;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Expression<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  Expression<OperatorInfo> exp = A;
  for (std::size_t i = 0; i < A.expression.size(); ++i) {
    exp.expression[i].push_back(B);
  }
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Operator<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  Expression<OperatorInfo> exp({{A}});
  return exp * B;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator+(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  Expression<OperatorInfo> exp = A;
  exp.expression.insert(exp.expression.end(), B.expression.begin(), B.expression.end());
  return exp;
}

template <class OperatorInfo>
Expression<OperatorInfo> operator*(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  Expression<OperatorInfo> exp;
  for (std::size_t i = 0; i < B.expression.size(); ++i) {
    exp = exp + A;
  }
  for (std::size_t i = 0; i < B.expression.size(); ++i) {
    for (std::size_t j = 0; j < A.expression.size(); ++j) {
      const auto index = i * A.expression.size() + j;
      exp.expression[index].insert(exp.expression[index].end(), B.expression[i].begin(), B.expression[i].end());
    }
  }
  return exp;
}

template <class OperatorInfo>
bool operator==(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  return A.expression == B.expression;
}


template <class OperatorInfo>
bool operator!=(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  return not (A.expression == B.expression);
}


template <class OperatorInfo>
bool operator<(const Expression<OperatorInfo> & A, const Expression<OperatorInfo> & B) {
  return A.expression < B.expression;
}
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// printing zone
//----------------------------------------------------------------------------------------------------------------------

template <class OperatorInfo>
std::string Expression<OperatorInfo>::print(const bool add_newline) const {
  std::stringstream ss;
  this->print(ss, add_newline);
  std::string ans = ss.str();
  return ans;
}


template <class OperatorInfo>
std::string Expression<OperatorInfo>::name() const {
  return this->print(false);
}

template <class OperatorInfo>
void Expression<OperatorInfo>::print(std::ostream& out, const bool add_newline) const {

  for (std::size_t i = 0; i < expression.size(); ++i) {
    if (expression[i].empty()) {
      continue;
    }
    if (i != 0) {
      out << " + ";
    }
    if (expression.size() != 1) {
      out << "(";
    }
    for (std::size_t j = 0; j < expression[i].size(); ++j) {
      if (j != (expression[i].size() - 1)) {
        out << expression[i][j].name() << " * ";
      } else {
        out << expression[i][j].name();
      }
    }
    if (expression.size() != 1) {
      out << ")";
    }
  }
  if (add_newline) {
    out << "\n";
  }
}

// end printing zone
//----------------------------------------------------------------------------------------------------------------------






} // end namespace operators
