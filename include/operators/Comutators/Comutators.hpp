#pragma once

#include <Expression/Expression.hpp>
#include <Utils/Utils.hpp>

// file of helpful comutator functions

namespace operators {

// returns zero, if 2 operators commute return this
template <class OperatorInfo>
inline Expression<OperatorInfo> zero_commutator() {
  return Expression<OperatorInfo>(vector_type<vector_type<Operator<OperatorInfo>>>(1,
                                                       vector_type<Operator<OperatorInfo>>(1, Operator<OperatorInfo>(0))
                                                      ));
}
// If the commutator of 2 operators is a number return this
template <class OperatorInfo>
inline Expression<OperatorInfo> numeric_commutator(const double n) {
  return Expression<OperatorInfo>(vector_type<vector_type<Operator<OperatorInfo>>>(1,
                                                       vector_type<Operator<OperatorInfo>>(1, Operator<OperatorInfo>(n))
                                                      ));
}
// if the commutator of 2 operatos is another operator return this
template <class OperatorInfo>
inline Expression<OperatorInfo>
operator_commutator(const ordering_value order, const OperatorInfo info) {
  return Expression<OperatorInfo>(vector_type<vector_type<Operator<OperatorInfo>>>(1,
                                  vector_type<Operator<OperatorInfo>>(1, Operator<OperatorInfo>(order, info))));
}

template <class OperatorInfo>
bool either_is_number(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  return A.is_number() or B.is_number();
}
// Takes a comutator of operators and extends it to commute all numbers
// As simplify of expressions assumes all numbers commute best to wrap all comutators with this function
template <class OperatorInfo>
Expression<OperatorInfo>
commute_numbers(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B,
                std::function<Expression<OperatorInfo>(const Operator<OperatorInfo> &,
                                                       const Operator<OperatorInfo> &)> commute) {
  if (either_is_number(A, B)) {
    return zero_commutator<OperatorInfo>();
  }
  return commute(A, B);
}

// Commute all
// ---------------------------------------------------------------------------------------------------------------------
// every operator is comutative
template <class OperatorInfo>
Expression<OperatorInfo> commute_all(const Operator<OperatorInfo> &, const Operator<OperatorInfo> &) {
  return zero_commutator<OperatorInfo>();
}

// Commute none
// ---------------------------------------------------------------------------------------------------------------------
struct GenericInfo {
  Type type = Type::UNSPECIFIED;
  std::string op_name;
  GenericInfo() = default;
  GenericInfo(std::string op_name) : op_name(op_name) {}
  std::string name() const {return op_name;}
  bool operator==(GenericInfo other) const {
    return type == other.type and op_name == other.op_name;
  }

  bool operator!=(GenericInfo other) const {
    return not (*this == other);
  }
  bool operator<(GenericInfo other) const {
    return std::tie(type, op_name) <
           std::tie(other.type, other.op_name);
  }
  bool isVacuumState() const {return false;}
  bool isHCVacuumState() const {return false;}
  bool match(const Fock1DInfo & other) const {return true;}
};

Expression<GenericInfo> commute_none_(const Operator<GenericInfo> & A, const Operator<GenericInfo> & B) {
  std::string com = "[" + A.name() + ", " + B.name() + "]";
  return Expression<GenericInfo>({{Operator<GenericInfo>(ordering_value(0), GenericInfo(com))}});
}

Expression<GenericInfo> commute_none(const Operator<GenericInfo> & A, const Operator<GenericInfo> & B) {
  return commute_numbers<GenericInfo>(A, B, commute_none_);
}

// Anti commute none
// ---------------------------------------------------------------------------------------------------------------------

Expression<GenericInfo> anticommute_none_(const Operator<GenericInfo> & A, const Operator<GenericInfo> & B) {
  std::string com = "{" + A.name() + ", " + B.name() + "}";
  return Expression<GenericInfo>({{Operator<GenericInfo>(ordering_value(0), com)}});
}

Expression<GenericInfo> anticommute_none(const Operator<GenericInfo> & A, const Operator<GenericInfo> & B) {
  return commute_numbers<GenericInfo>(A, B, anticommute_none_);
}
// ---------------------------------------------------------------------------------------------------------------------


// Boson comutator
// ---------------------------------------------------------------------------------------------------------------------
template <class OperatorInfo>
Expression<OperatorInfo> boson_commutator_(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  if (is_anihilation_op<OperatorInfo>(A) and is_creation_op<OperatorInfo>(B)) {
    if (A.info().match(B.info())) {
      return Expression<OperatorInfo>({{number<OperatorInfo>(1)}});
    } else {
      return zero_commutator<OperatorInfo>();
    }
  }
  throw std::logic_error("Shouldn't be commuting anything else");
}

template <class OperatorInfo>
Expression<OperatorInfo> boson_commutator(const Operator<OperatorInfo> & A, const Operator<OperatorInfo> & B) {
  return commute_numbers<OperatorInfo>(A, B, boson_commutator_<OperatorInfo>);
}
// ---------------------------------------------------------------------------------------------------------------------



} // End namespace operators
