#pragma once


namespace operators {

struct GenericInfo {
//  Type type = Type::UNSPECIFIED;
  std::string op_name;
  GenericInfo() = default;
  GenericInfo(std::string op_name) : op_name(op_name) {}
  std::string name() const {return op_name;}
  bool operator==(GenericInfo other) const {
    return /*type == other.type and*/ op_name == other.op_name;
  }

  bool operator!=(GenericInfo other) const {
    return not (*this == other);
  }
  bool operator<(GenericInfo other) const {
    return op_name < other.op_name; //std::tie(type, op_name) <
           //std::tie(other.type, other.op_name);
  }
  bool isVacuumState() const {return false;}
  bool isHCVacuumState() const {return false;}
  bool match(const GenericInfo & other) const {return true;}
};

}

