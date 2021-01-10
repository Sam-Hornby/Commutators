#pragma once


namespace operators {

struct EmptyInfo {
  EmptyInfo() {
    throw std::logic_error("Constructing Empty Info");
  }
  std::string name() const {
    std::abort();
  }
  bool operator==(EmptyInfo other) const {
    std::abort();
  }

  bool operator!=(EmptyInfo other) const {
    return not (*this == other);
  }
  bool operator<(EmptyInfo other) const {
    std::abort();
  }
  bool isVacuumState() const {std::abort();}
  bool isHCVacuumState() const {std::abort();}
  bool match(const EmptyInfo & other) const {std::abort();}
};

}



