#ifndef _operator_hpp_
#define _operator_hpp_

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <string>
#include "struct_ids.hpp"



namespace operators {

struct ImaginaryNumber {
  double value;
};

template <class OperatorInfo>
struct IsNumberVisitor : public boost::static_visitor<bool>{
  bool operator() (const double) const {return true;}
  bool operator() (const ImaginaryNumber) const {return true;}
  bool operator() (const OperatorInfo) const {return false;}
};

template <class OperatorInfo>
struct NameVisitor : public boost::static_visitor<std::string>{
  std::string operator() (const double &a) const {return std::to_string(a);}
  std::string operator() (const ImaginaryNumber) const {return "i";}
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

struct RealVisitor : public boost::static_visitor<double>{
  template <typename T>
  double operator() (const T) const {
    throw std::logic_error("Trying to access value on operator or Imaginary");
  }
  double operator() (const double value) const {return value;}
};

struct ImaginaryVisitor : public boost::static_visitor<ImaginaryNumber>{
  template <typename T>
  ImaginaryNumber operator() (const T) const {
    throw std::logic_error("Trying to access value on operator or Imaginary");
  }
  ImaginaryNumber operator() (const ImaginaryNumber value) const {return value;}
};

struct IsImaginary : public boost::static_visitor<bool>{
  template <typename T>
  double operator() (const T) const {return false;}
  bool operator() (const ImaginaryNumber) const {return true;}
};

template <class OperatorInfo>
struct Operator {
  ordering_value order;  // when sorting this determines greater than or equal to
  //OperatorInfo info;     // should be unique for each operator TODO make const
  // if this is set operator is a number, comutators are automatically assumed zero, op_id is irelevant
  boost::variant<double, ImaginaryNumber, OperatorInfo> data;

  Operator() = default;
  Operator(ordering_value order, OperatorInfo info) : order(order), data(info) {}
  Operator(double v) : order(ordering_value(std::numeric_limits<int>::min())), data(v) {}

  std::string name() const {
    return boost::apply_visitor(NameVisitor<OperatorInfo>{}, data);
  }

  bool is_number() const {
    return boost::apply_visitor(IsNumberVisitor<OperatorInfo>{}, data);
  }

  bool is_imaginary() const {
    return boost::apply_visitor(IsImaginary{}, data);
  }

  bool is_real() const {
    return is_number() and not is_imaginary();
  }

  double value() const {
    return boost::apply_visitor(RealVisitor{}, data);
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

//******************************************************************
// All definitions below
//******************************************************************

template <class OperatorInfo>
Operator<OperatorInfo> number(const double n) {
  return Operator<OperatorInfo>(n);
}

} // end namespace

#endif // header guard
