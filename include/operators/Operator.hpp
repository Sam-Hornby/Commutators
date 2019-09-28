#ifndef _operator_hpp_
#define _operator_hpp_

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <string>
#include "struct_ids.hpp"
#include "ImaginaryNumber.hpp"



namespace operators {

template <class OperatorInfo>
struct IsNumberVisitor : public boost::static_visitor<bool>{
  bool operator() (const ComplexNumber) const {return true;}
  bool operator() (const OperatorInfo) const {return false;}
};

template <class OperatorInfo>
struct NameVisitor : public boost::static_visitor<std::string>{
  std::string operator() (const ComplexNumber &a) const {return a.name();}
  std::string operator() (const OperatorInfo &info) const {return info.name();}
};

template <class OperatorInfo>
struct InfoVisitor : public boost::static_visitor<OperatorInfo &>{
  template <typename T>
  OperatorInfo & operator() (const T) const {
    throw std::logic_error("Trying to access info on number");
  }
  OperatorInfo & operator() (OperatorInfo &info) const {return info;}
};

template <class OperatorInfo>
struct ConstInfoVisitor : public boost::static_visitor<const OperatorInfo &>{
  template <typename T>
  const OperatorInfo & operator() (const T) const {
    throw std::logic_error("Trying to access info on number");
  }
  const OperatorInfo & operator() (const OperatorInfo &info) const {return info;}
};

struct NumberVisitor : public boost::static_visitor<ComplexNumber>{
  template <typename T>
  ComplexNumber operator() (const T) const {
    throw std::logic_error("Trying to access value on operator or Imaginary");
  }
  ComplexNumber operator() (const ComplexNumber value) const {return value;}
};

template <class OperatorInfo>
struct Operator {
  ordering_value order;  // when sorting this determines greater than or equal to
  //OperatorInfo info;     // should be unique for each operator TODO make const
  // if this is set operator is a number, comutators are automatically assumed zero, op_id is irelevant
  boost::variant<ComplexNumber, OperatorInfo> data;

  Operator() = default;
  Operator(ordering_value order, OperatorInfo info) : order(order), data(info) {}
  Operator(ComplexNumber i) : order(ordering_value(std::numeric_limits<int>::min())), data(i) {}


  std::string name() const {
    return boost::apply_visitor(NameVisitor<OperatorInfo>{}, data);
  }

  bool is_number() const {
    return boost::apply_visitor(IsNumberVisitor<OperatorInfo>{}, data);
  }

  ComplexNumber value() const {
    return boost::apply_visitor(NumberVisitor{}, data);
  }

  OperatorInfo & info() {
    return boost::apply_visitor(InfoVisitor<OperatorInfo>{}, data);
  }

  const OperatorInfo & info() const {
    return boost::apply_visitor(ConstInfoVisitor<OperatorInfo>{}, data);
  }

  bool operator<(Operator other) const {
    return order < other.order;
  }

  bool operator>(Operator other) const {
    return order > other.order;
  }

  bool operator==(Operator other) const {
    return (order == other.order) and (data == data.info);
  }

  bool operator!=(Operator other) const {
    return !(*this == other);
  }

};

template <class OperatorInfo>
Operator<OperatorInfo> number(const double n);

template <class OperatorInfo>
Operator<OperatorInfo> im_number(const double n) {
  return Operator<OperatorInfo>(ImaginaryNumber(n));
}

//******************************************************************
// All definitions below
//******************************************************************

template <class OperatorInfo>
Operator<OperatorInfo> number(const double n) {
  return Operator<OperatorInfo>(n);
}

} // end namespace

#endif // header guard
