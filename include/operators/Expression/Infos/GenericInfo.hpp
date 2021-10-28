#pragma once
#include "Fock1D.hpp"

namespace operators {

struct GenericInfo {
  //  Type type = Type::UNSPECIFIED;
  std::string op_name;
  GenericInfo() = default;
  GenericInfo(std::string op_name) : op_name(op_name) {}
  std::string name() const { return op_name; }
  bool operator==(GenericInfo other) const {
    return /*type == other.type and*/ op_name == other.op_name;
  }

  bool operator!=(GenericInfo other) const { return not(*this == other); }
  bool operator<(GenericInfo other) const {
    return op_name < other.op_name; // std::tie(type, op_name) <
                                    // std::tie(other.type, other.op_name);
  }
  bool isVacuumState() const { return false; }
  bool isHCVacuumState() const { return false; }
  bool match(const GenericInfo &other) const { return true; }
  Type get_type() const { std::abort(); } // need to create this ?
  void complex_conjugate() {
    if (op_name[op_name.size() - 1] == '!') {
      op_name = op_name.substr(0, op_name.size() - 1);
    } else {
      op_name = op_name + "!";
    }
  }
};

} // namespace operators
