#pragma once

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <string>
#include "struct_ids.hpp"
#include <Numbers/ImaginaryNumber.hpp>
#include <Numbers/NamedNumber.hpp>
#include <Numbers/CompositeNumbers.hpp>



namespace operators {

namespace {

template <class OperatorInfo>
struct IsNumberVisitor : public boost::static_visitor<bool>{
  bool operator() (const ComplexNumber &) const {return true;}
  bool operator() (const OperatorInfo &) const {return false;}
  bool operator() (const NamedNumber &) const {return true;}
  bool operator() (const CompositeNumber &) const  {return true;}
};

template <class OperatorInfo>
struct hasValueVisitor : public boost::static_visitor<bool>{
  bool operator() (const ComplexNumber &) const {return true;}
  bool operator() (const OperatorInfo &) const {return false;}
  bool operator() (const NamedNumber &) const {return false;}
  bool operator() (const CompositeNumber &) const {return false;}
};

template <class OperatorInfo>
struct NameVisitor : public boost::static_visitor<std::string>{
  template <typename T>
  std::string operator() (const T &a) const {return a.name();}
};

template <class OperatorInfo>
struct InfoVisitor : public boost::static_visitor<OperatorInfo &>{
  template <typename T>
  OperatorInfo & operator() (const T &) const {
    throw std::logic_error("Trying to access info on number");
  }
  OperatorInfo & operator() (OperatorInfo &info) const {return info;}
};

template <class OperatorInfo>
struct ConstInfoVisitor : public boost::static_visitor<const OperatorInfo &>{
  template <typename T>
  const OperatorInfo & operator() (const T&) const {
    throw std::logic_error("Trying to access info on number");
  }
  const OperatorInfo & operator() (const OperatorInfo &info) const {return info;}
};

struct NumberVisitor : public boost::static_visitor<ComplexNumber>{
  template <typename T>
  ComplexNumber operator() (const T&) const {
    throw std::logic_error("Trying to access value on operator");
  }
  ComplexNumber operator() (const ComplexNumber &value) const {return value;}
};

struct NamedNumberVisitor : public boost::static_visitor<NamedNumber>{
  template <typename T>
  NamedNumber operator() (const T&) const {
    throw std::logic_error("Trying to access value on operator");
  }
  NamedNumber operator() (const NamedNumber& value) const {return value;}
};

template <class T>
struct isAVisitor {
  template <class U>
  bool operator() (const U &) const {return false;}
  bool operator() (const T &) const {return true;}
};

template <class T>
struct getAsVisitor : boost::static_visitor<T &> {
  T & operator() (T &t) const {
    return t;
  }
  template <class U>
  T & operator() (const U &) const {
    throw std::logic_error("Incorrect type for get as");
  }
};

} // namespace
template <class OperatorInfo>
struct Operator {
  ordering_value order;  // when sorting this determines greater than or equal to
  //OperatorInfo info;     // should be unique for each operator TODO make const
  // if this is set operator is a number, comutators are automatically assumed zero, op_id is irelevant
  boost::variant<ComplexNumber, OperatorInfo, NamedNumber, CompositeNumber> data;

  Operator() = default;
  Operator(ordering_value order, OperatorInfo info) :
               order(order), data(std::move(info)) {}
  Operator(ComplexNumber i) :
               order(ordering_value(std::numeric_limits<int>::min())), data(std::move(i)) {}
  Operator(NamedNumber i) :
               order(ordering_value(std::numeric_limits<int>::min())), data(std::move(i)) {}
  Operator(CompositeNumber i) :
               order(ordering_value(std::numeric_limits<int>::min())), data(std::move(i)) {}


  std::string name() const {
    return boost::apply_visitor(NameVisitor<OperatorInfo>{}, data);
  }

  bool is_number() const {
    return boost::apply_visitor(IsNumberVisitor<OperatorInfo>{}, data);
  }

  bool is_evaluated_number() const {
    return boost::apply_visitor(hasValueVisitor<OperatorInfo>{}, data);
  }

  bool is_composite_number() const {
    return boost::apply_visitor(isAVisitor<CompositeNumber>(), data);
  }

  CompositeNumber & composite_number() {
    return boost::apply_visitor(getAsVisitor<CompositeNumber>(), data);
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

  NamedNumber named_number() const {
    return boost::apply_visitor(NamedNumberVisitor{}, data);
  }

  bool operator<(Operator other) const {
    return order < other.order;
  }

  bool operator>(Operator other) const {
    return order > other.order;
  }

  bool operator==(Operator other) const {
    return (order == other.order) and (data == other.data);
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

template <class OperatorInfo>
Operator<OperatorInfo> named_number(const char n) {
  return Operator<OperatorInfo>(NamedNumber(n));
}

//******************************************************************
// All definitions below
//******************************************************************

template <class OperatorInfo>
Operator<OperatorInfo> number(const double n) {
  return Operator<OperatorInfo>(n);
}


} // end namespace
