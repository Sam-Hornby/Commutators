#pragma once

#include "CompositeNumbers.hpp"
#include "NamedNumber.hpp"
#include "ImaginaryNumber.hpp"
#include <Expression/Expression.hpp>
#include <Expression/Infos/EmptyInfo.hpp>
#include <Expression/Simplify.hpp>

namespace operators {

template <class T>
static const T * getAs(const CompositeNumber &a) {
  if (a.expr->getClassId() != T::classId) {
    return nullptr;
  }
  return reinterpret_cast<const T *>(a.expr.get());
}

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

  std::vector<const CompositeNumber *> get_children() const override {
    return {};
  }

  CompositeNumber simplify() const override {
    auto result = clone();
    auto result_ = dynamic_cast<SingleExpr<Expression<EmptyInfo>> *>(result.get());
    if (result_) {
      result_->item = simplify_numbers(result_->item);
      if (result_->item.expression.size() == 1 and
          result_->item.expression[0].size() == 1 and
          result_->item.expression[0][0].is_evaluated_number()) {
        return CompositeNumber(std::make_unique<SingleExpr<ComplexNumber>>(
          result_->item.expression[0][0].value()));

      }
    }
    return CompositeNumber(std::move(result));
  }
};

static boost::optional<ComplexNumber>
getAsComplexNumber(const CompositeNumber &a) {
  if (const auto * c = getAs<SingleExpr<ComplexNumber>>(a)) {
    return c->item;
  }
  return boost::none;
}

struct UnaryInfo {
  std::string name;
  bool before;
  bool needsBrackets;
  std::function<std::complex<double>(std::complex<double>)> transform;
};

const std::vector<UnaryInfo> UnarySymbols = {
  {"\u221A", true, false, [](std::complex<double> a) {return std::sqrt(a);}},
  {"exp", true, true, [](std::complex<double> a) {return std::exp(a);}},
  {"\u00B2", false, false,[](std::complex<double> a) {return std::pow(a, 2);}},
};

template <unsigned identifier>
struct UnaryCompositeNumber : public CompositeNumberBase {
  static const std::size_t classId;
  static std::string getSymbol() {
    return UnarySymbols[identifier].name;
  }
  CompositeNumber operand;
  
  explicit UnaryCompositeNumber(CompositeNumber first) :
         operand(std::move(first)) {}
  static CompositeNumber create(CompositeNumber operand) {
    return CompositeNumber(std::make_unique<UnaryCompositeNumber>(operand));
  }

  std::string name() const override {
    const bool needsBrackets = UnarySymbols[identifier].needsBrackets;
    if (UnarySymbols[identifier].before) {
      if (needsBrackets) {
        return getSymbol() + std::string("(") + operand.name() + ")";
      }
      return getSymbol() + operand.name();
    }
    if (needsBrackets) {
      return std::string("(") + operand.name() + ")" + getSymbol();
    }
    return operand.name() + getSymbol();
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
  std::vector<const CompositeNumber *> get_children() const override {
    return {&operand};
  }
  CompositeNumber simplify() const override {
    auto simp_op = operand.simplify();
    auto c = getAsComplexNumber(simp_op);
    if (c) {
      const auto &uInfo = UnarySymbols[identifier];
      return CompositeNumber(
          std::make_unique<SingleExpr<ComplexNumber>>(
            ComplexNumber(uInfo.transform(c.get().value))
          ));
    };
    return CompositeNumber(
      std::make_unique<UnaryCompositeNumber<identifier>>(std::move(simp_op)));
  }
};

struct BinaryInfo {
  std::string symbol;
  bool before;
  std::function<std::complex<double>(std::complex<double>,
                                     std::complex<double>)> transform;
};

const std::vector<BinaryInfo> BinarySymbols = {
  {"+", false, [](std::complex<double> a, std::complex<double> b) {
                  return a + b;
                }},
  {"-", false, [](std::complex<double> a, std::complex<double> b) {
                  return a - b;
                }},
  {"*", false, [](std::complex<double> a, std::complex<double> b) {
                  return a * b;
                }},
  {"/", false, [](std::complex<double> a, std::complex<double> b) {
                  return a / b;
                }},
};

template <unsigned id>
struct BinaryCompositeNumber : public CompositeNumberBase {
  static const std::size_t classId;
  static std::string getSymbol() {
    return BinarySymbols[id].symbol;
  }
  CompositeNumber first, second;
  explicit BinaryCompositeNumber(CompositeNumber first, CompositeNumber second) :
       first(std::move(first)), second(std::move(second)) {}

  static CompositeNumber create(CompositeNumber first, CompositeNumber second) {
    return CompositeNumber(std::make_unique<BinaryCompositeNumber<id>>(
                std::move(first), std::move(second)));
  }
  std::string name() const override {
    if (BinarySymbols[id].before) {
      return getSymbol() + std::string("(") + first.name() + ", " + second.name() + ")"; 
    }
    return std::string("(") + first.name() + getSymbol() + second.name() + ")";
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

  std::vector<const CompositeNumber *> get_children() const override {
    return {&first, &second};
  }
  CompositeNumber simplify() const override {
    auto a = first.simplify();
    auto b = second.simplify();
    auto A = getAsComplexNumber(a);
    auto B = getAsComplexNumber(b);
    if (A and B) {
      const auto &bInfo = BinarySymbols[id];
      return CompositeNumber(
          std::make_unique<SingleExpr<ComplexNumber>>(
            ComplexNumber(bInfo.transform(A.get().value,
                                          B.get().value))
          ));
    };
    return CompositeNumber(
      std::make_unique<BinaryCompositeNumber<id>>(
        std::move(a), std::move(b)));
  }
};

template <class OperatorInfo>
struct ConversionVisitor : public boost::static_visitor<Operator<EmptyInfo>> {
  template <class T>
  Operator<EmptyInfo> operator() (T &number) const {
    return Operator<EmptyInfo>(std::move(number));
  }
  Operator<EmptyInfo> operator() (OperatorInfo & info) const {
    std::abort();
  }
};

template <class OperatorInfo>
Operator<EmptyInfo> convertTerm(Operator<OperatorInfo> term) {
  return boost::apply_visitor(ConversionVisitor<OperatorInfo>{}, term.data);
}

template <class OperatorInfo>
Expression<EmptyInfo> convertNumbersToEmptyInfo(Expression<OperatorInfo> exp) {
  // All of expression must be numbers. If Any operators are info type then will
  // throw error
  Expression<EmptyInfo> result;
  result.expression.reserve(exp.expression.size());
  for (auto &mul_terms : exp.expression) {
    result.expression.push_back(vector_type<Operator<EmptyInfo>>());
    auto &current = result.expression.back();
    current.reserve(mul_terms.size());
    for (auto &term : mul_terms) {
      current.push_back(convertTerm<OperatorInfo>(std::move(term)));
    }
  }
  return result;
}



typedef SingleExpr<NamedNumber> NamedNumberExpr;
typedef SingleExpr<ComplexNumber> ComplexNumberExpr;
typedef SingleExpr<Expression<EmptyInfo>> ExpressionExpr;

typedef UnaryCompositeNumber<0> SquareRootExpr;
typedef UnaryCompositeNumber<1> ExponentialExpr;
typedef UnaryCompositeNumber<2> SquareExpr;

typedef BinaryCompositeNumber<0> AddExpr;
typedef BinaryCompositeNumber<1> SubExpr;
typedef BinaryCompositeNumber<2> MulExpr;
typedef BinaryCompositeNumber<3> DivExpr;

template <> const std::size_t NamedNumberExpr::classId = __COUNTER__;
template <> const std::size_t ComplexNumberExpr::classId = __COUNTER__;
template <> const std::size_t ExpressionExpr::classId = __COUNTER__;

template <> const std::size_t SquareRootExpr::classId = __COUNTER__;
template <> const std::size_t ExponentialExpr::classId = __COUNTER__;
template <> const std::size_t SquareExpr::classId = __COUNTER__;


template <> const std::size_t AddExpr::classId = __COUNTER__;
template <> const std::size_t SubExpr::classId = __COUNTER__;
template <> const std::size_t MulExpr::classId = __COUNTER__;
template <> const std::size_t DivExpr::classId = __COUNTER__;

}
