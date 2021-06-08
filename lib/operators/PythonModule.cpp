#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <Expression/Operator.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <Expression/Expression.hpp>


namespace py = pybind11;

namespace operators {

namespace {

char get_as_char(std::string n) {
  if (n.size() != 1) {
    throw std::logic_error("Name must be 1 character in size");
  }
  return n[0];
}

}

template <class Info>
Expression<Info> number_(const double n) {
  return {{{number<Info>(n)}}};
}

template <class Info>
Expression<Info> named_number_(std::string n) {
  return {{{named_number<Info>(get_as_char(n))}}};
}

Expression<Fock1DInfo> create_fock_op(short int x_coordinate,
                                    Type type,
                                    std::string symbol) {
  if (not isFockOpType(type)) {
    throw std::logic_error("Can only use Fock op types for this creation function");
  }
  
  return {{{Operator<Fock1DInfo>(ordering_value(0),
                              Fock1DInfo(x_coordinate, 0, type,
                                         get_as_char(symbol)))}}};
}

Expression<Fock1DInfo> create_state_vector(short int x, short int state, Type type) {
  if (not isVectorType(type)) {
    throw std::logic_error("Can only use vector types for this create function");
  }
  auto order = ordering_value(type == Type::STATE_VECTOR ? 1000 : -1000);
  return {{{Operator<Fock1DInfo>(order, Fock1DInfo(x, state, type, 'v'))}}};
}

PYBIND11_MODULE(FockOperators, handle) {

  py::enum_<Type>(handle, "FockType")
        .value("STATE_VECTOR", Type::STATE_VECTOR)
        .value("HC_STATE_VECTOR", Type::HC_STATE_VECTOR)
        .value("CREATION_OPERATOR", Type::CREATION_OPERATOR)
        .value("ANIHILATION_OPERATOR", Type::ANIHILATION_OPERATOR)
        .value("UNSPECIFIED", Type::UNSPECIFIED)
        .export_values();


  py::class_<Expression<Fock1DInfo>>(handle, "FockExpression")
        //.def(py::init<int, int, Type, char>())
        .def("__str__", &Expression<Fock1DInfo>::name)
        .def("__add__", &Expression<Fock1DInfo>::__add__)
        .def("__mul__", &Expression<Fock1DInfo>::__mul__)
        .def("__sub__", &Expression<Fock1DInfo>::__sub__);

  handle.def("number", &number_<Fock1DInfo>);
  handle.def("named_number", &named_number_<Fock1DInfo>);
  handle.def("fock_op", &create_fock_op);
  handle.def("state_vector", &create_state_vector);

  // Think maybe should use factories instead of constructors to help create numbers

}

}
