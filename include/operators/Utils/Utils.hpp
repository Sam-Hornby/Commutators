#pragma once

#include <Expression/Operator.hpp>
#include <Expression/struct_ids.hpp>
#include <Expression/Expression.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <cmath>
#include <math.h>
#include <Numbers/CompositeNumberExpressions.hpp>

namespace operators {

template <class OperatorInfo>
inline Operator<OperatorInfo> vacuum_state() {
  return Operator<OperatorInfo>(ordering_value(0), OperatorInfo::vacuumState());
}

inline Operator<Fock1DInfo> creation_op(const int info_value) {
  return Operator<Fock1DInfo>(ordering_value(0),
                  Fock1DInfo(info_value, Type::CREATION_OPERATOR));
}

inline Operator<Fock1DInfo> anihilation_op(const int info_value) {
  return Operator<Fock1DInfo>(ordering_value(0),
                  Fock1DInfo(info_value, Type::ANIHILATION_OPERATOR));
}

inline Expression<Fock1DInfo> normalised_n_occupied_state(unsigned n, unsigned creation_op_index) {
  Expression<Fock1DInfo> exp;
  exp.expression.push_back({vacuum_state<Fock1DInfo>()});
  double normalisation_factor = 1.0 / std::sqrt(tgamma(static_cast<double>(n + 1)));
  for (unsigned i = 0; i < n; ++i) {
    exp = creation_op(creation_op_index) * exp;
  }
  return number<Fock1DInfo>(normalisation_factor) * exp;
}

inline Expression<Fock1DInfo> normalised_n_occupied_ops(unsigned n, unsigned creation_op_index) {
  Expression<Fock1DInfo> exp;
  exp.expression.push_back({number<Fock1DInfo>(1.0)});
  double normalisation_factor = 1.0 / std::sqrt(tgamma(static_cast<double>(n + 1)));
  for (unsigned i = 0; i < n; ++i) {
    exp = creation_op(creation_op_index) * exp;
  }
  return number<Fock1DInfo>(normalisation_factor) * exp;
}
namespace {

template <class Info>
struct ConjugateVisitor {
  void operator() (ComplexNumber &num) const {
    num.value = std::conj(num.value);
  }
  void operator() (Info &info) const {
    info.complex_conjugate();
  }
  void operator() (NamedNumber &num) const {
    num.complex_conjugated = num.complex_conjugated xor true;
  }
  void operator() (CompositeNumber &num) const {
    num = ConjugateExpr::create(std::move(num));
  }
};

}

template <class OperatorInfo>
Operator<OperatorInfo> hermition_conjugate(Operator<OperatorInfo> op) {
  std::visit(ConjugateVisitor<OperatorInfo>{}, op.data);
  return op;
}

template <class OperatorInfo>
Expression<OperatorInfo> hermition_conjugate(const Expression<OperatorInfo> & exp) {
  Expression<OperatorInfo> new_exp;
  new_exp.expression.resize(exp.expression.size());
  for (unsigned i = 0; i < new_exp.expression.size(); ++i) {
    for (int j = exp.expression[i].size() - 1; j >= 0; --j) {
      new_exp.expression[i].push_back(hermition_conjugate<OperatorInfo>(exp.expression[i][j]));
    }
  }
  return new_exp;
}

template <class OperatorInfo>
bool is_state_vector(const Operator<OperatorInfo> & A) {
  if (A.is_number()) {return false;}
  return A.info().type == Type::STATE_VECTOR;
}
template <class OperatorInfo>
bool is_hc_state_vector(const Operator<OperatorInfo> & A) {
  if (A.is_number()) {return false;}
  return A.info().type == Type::HC_STATE_VECTOR;
}
template <class OperatorInfo>
bool is_creation_op(const Operator<OperatorInfo> & A) {
  if (A.is_number()) {return false;}
  return A.info().type == Type::CREATION_OPERATOR;
}
template <class OperatorInfo>
bool is_anihilation_op(const Operator<OperatorInfo> & A) {
  if (A.is_number()) {return false;}
  return A.info().type == Type::ANIHILATION_OPERATOR;
}
template <class OperatorInfo>
bool is_unspecified_op(const Operator<OperatorInfo> & A) {
  if (A.is_number()) {return false;}
  return A.info().type == Type::UNSPECIFIED;
}

} // end namspace

