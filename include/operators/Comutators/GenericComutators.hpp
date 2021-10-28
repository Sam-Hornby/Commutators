#pragma once
#include "Comutators.hpp"
#include <Expression/Infos/GenericInfo.hpp>

namespace operators {

// Commute none
// ---------------------------------------------------------------------------------------------------------------------

Expression<GenericInfo> commute_none_(const Operator<GenericInfo> &A,
                                      const Operator<GenericInfo> &B) {
  std::string com = "[" + A.name() + ", " + B.name() + "]";
  return Expression<GenericInfo>(
      {{Operator<GenericInfo>(ordering_value(0), GenericInfo(com))}});
}

Expression<GenericInfo> commute_none(const Operator<GenericInfo> &A,
                                     const Operator<GenericInfo> &B) {
  return commute_numbers<GenericInfo>(A, B, commute_none_);
}

// Anti commute none
// ---------------------------------------------------------------------------------------------------------------------

Expression<GenericInfo> anticommute_none_(const Operator<GenericInfo> &A,
                                          const Operator<GenericInfo> &B) {
  std::string com = "{" + A.name() + ", " + B.name() + "}";
  return Expression<GenericInfo>(
      {{Operator<GenericInfo>(ordering_value(0), com)}});
}

Expression<GenericInfo> anticommute_none(const Operator<GenericInfo> &A,
                                         const Operator<GenericInfo> &B) {
  return commute_numbers<GenericInfo>(A, B, anticommute_none_);
}
// ---------------------------------------------------------------------------------------------------------------------

} // namespace operators
