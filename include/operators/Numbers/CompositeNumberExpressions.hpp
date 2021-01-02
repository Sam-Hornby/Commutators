#pragma once

#include "CompositeNumbers.hpp"
#include "NamedNumber.hpp"
#include "ImaginaryNumber.hpp"
#include <typeindex>

namespace operators {


template <class T>
struct SingleExpr : public CompositeNumberBase {
  static const std::size_t classId;
  T item;
  explicit SingleExpr(T item) : item(std::move(item)) {}
  static CompositeNumber create(T item) {
    return CompositeNumber(std::make_unique<SingleExpr<T>>(std::move(item)));
  }
  std::string name() const override {return item.name();}
  bool operator==(const CompositeNumberBase &other) const override {
    auto * other_p = dynamic_cast<const SingleExpr<T> *>(&other);
    if (!other_p) {
      return false;
    }
    return item == other_p->item;
  }
  bool operator<(const CompositeNumberBase &other) const override {
    if (this->getClassId() != other.getClassId()) {
      return this->getClassId() < other.getClassId();
    }
    auto * other_p = dynamic_cast<const SingleExpr<T> *>(&other);
    return item < other_p->item;
  }  
  std::unique_ptr<CompositeNumberBase> clone() const override {
    return std::make_unique<SingleExpr<T>>(*this);
  }
  
  std::size_t getClassId() const override {
    return classId;
  }

};


const std::vector<std::string> UnarySymbols = {
  "sqrt",
  "exp",
};

template <unsigned identifier>
struct UnaryCompositeNumber : public CompositeNumberBase {
  static const std::size_t classId;
  static std::string getSymbol() {
    return UnarySymbols[identifier];
  }
  CompositeNumber operand;
  
  explicit UnaryCompositeNumber(CompositeNumber first) :
         operand(std::move(first)) {}
  static CompositeNumber create(CompositeNumber operand) {
    return CompositeNumber(std::make_unique<UnaryCompositeNumber>(operand));
  }

  std::string name() const override {
    return getSymbol() + std::string("(") + operand.name() + ")"; 
  }
   
  bool operator==(const CompositeNumberBase &other) const override {
    auto * other_p = dynamic_cast<const UnaryCompositeNumber<identifier> *>(&other);
    if (!other_p) { return false; }
    return operand == other_p->operand;  
  }
  bool operator<(const CompositeNumberBase &other) const override {
    if (this->getClassId() != other.getClassId()) {
      return this->getClassId() < other.getClassId();
    }
    auto * other_p = dynamic_cast<const UnaryCompositeNumber<identifier> *>(&other);
    return operand < other_p->operand; 
  }
  std::unique_ptr<CompositeNumberBase> clone() const override {
    return std::make_unique<UnaryCompositeNumber<identifier>>(*this);
  } 
  std::size_t getClassId() const override {
    return classId;
  } 
};

const std::vector<std::string> BinarySymbols = {
  "add",
  "sub",
  "mul",
  "div",
};

template <unsigned id>
struct BinaryCompositeNumber : public CompositeNumberBase {
  static const std::size_t classId;
  static std::string getSymbol() {
    return BinarySymbols[id];
  }
  CompositeNumber first, second;
  explicit BinaryCompositeNumber(CompositeNumber first, CompositeNumber second) :
       first(std::move(first)), second(std::move(second)) {}

  static CompositeNumber create(CompositeNumber first, CompositeNumber second) {
    return CompositeNumber(std::make_unique<BinaryCompositeNumber<id>>(
                std::move(first), std::move(second)));
  }
  std::string name() const override {
    return getSymbol() + std::string("(") + first.name() + ", " + second.name() + ")"; 
  }
   
  bool operator==(const CompositeNumberBase &other) const override {
    auto * other_p = dynamic_cast<const BinaryCompositeNumber<id> *>(&other);
    if (!other_p) { return false; }
    return std::tie(first, second) == std::tie(other_p->first, other_p->second);  
  }
  bool operator<(const CompositeNumberBase &other) const override {
    if (this->getClassId() != other.getClassId()) {
      return this->getClassId() < other.getClassId();
    }
    auto * other_p = dynamic_cast<const BinaryCompositeNumber<id> *>(&other);
    return std::tie(first, second) < std::tie(other_p->first, other_p->second); 
  }
  std::unique_ptr<CompositeNumberBase> clone() const override {
    return std::make_unique<BinaryCompositeNumber<id>>(*this);
  } 
  std::size_t getClassId() const override {
    return classId;
  } 
  
};

template <class T>
CompositeNumber createComposite(T expr) {
  return CompositeNumber(std::make_unique<SingleExpr<T>>(expr));
}

typedef SingleExpr<NamedNumber> NamedNumberExpr;
typedef SingleExpr<ComplexNumber> ComplexNumberExpr;

typedef UnaryCompositeNumber<0> SquareRootExpr;
typedef UnaryCompositeNumber<1> ExponentialExpr;

typedef BinaryCompositeNumber<0> AddExpr;
typedef BinaryCompositeNumber<1> SubExpr;
typedef BinaryCompositeNumber<2> MulExpr;
typedef BinaryCompositeNumber<3> DivExpr;

template <> const std::size_t NamedNumberExpr::classId = __COUNTER__;
template <> const std::size_t ComplexNumberExpr::classId = __COUNTER__;

template <> const std::size_t SquareRootExpr::classId = __COUNTER__;
template <> const std::size_t ExponentialExpr::classId = __COUNTER__;


template <> const std::size_t AddExpr::classId = __COUNTER__;
template <> const std::size_t SubExpr::classId = __COUNTER__;
template <> const std::size_t MulExpr::classId = __COUNTER__;
template <> const std::size_t DivExpr::classId = __COUNTER__;

}
