#pragma once

#include <string>
#include <memory>


namespace operators {


struct CompositeNumberBase {
  virtual std::string name() const = 0;
  virtual bool operator==(const CompositeNumberBase &) const = 0;
  virtual bool operator<(const CompositeNumberBase &) const = 0;
  virtual ~CompositeNumberBase() {}
  virtual std::unique_ptr<CompositeNumberBase> clone() const = 0;
  virtual std::size_t getClassId() const = 0;
};


struct CompositeNumber {
  std::unique_ptr<CompositeNumberBase> expr;
  
  explicit CompositeNumber(std::unique_ptr<CompositeNumberBase> expr) :
    expr(std::move(expr)) {}


  CompositeNumber(CompositeNumber &&) = default;
  CompositeNumber(const CompositeNumber &other) : expr(other.expr->clone()) {} 
  CompositeNumber& operator=(CompositeNumber other) {
    std::swap(*this, other);
    return *this;
  }

  std::string name() const {
    return expr->name();
  }
  bool operator==(const CompositeNumber &other) const {
    return *expr == *other.expr;
  }
  bool operator!= (const CompositeNumber &other) const {
    return not (*this == other);
  }
  bool operator<(const CompositeNumber &other) const {
    return *expr < *other.expr;
  }

};


}
