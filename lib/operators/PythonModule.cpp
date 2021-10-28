#include <Comutators/BosonComutators.hpp>
#include <Expression/Evaluate.hpp>
#include <Expression/Expression.hpp>
#include <Expression/Infos/Fock1D.hpp>
#include <Expression/Operator.hpp>
#include <Expression/Ordering.hpp>
#include <Substitutions/Substitutions.hpp>
#include <Utils/Utils.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace operators {

namespace {

using PyType = Expression<Fock1DInfo>;

char get_as_char(std::string n) {
  if (n.size() != 1) {
    throw std::logic_error("Name must be 1 character in size");
  }
  return n[0];
}

} // namespace

template <class Info>
Expression<Info> number_(const double n) {
  return {{{number<Info>(n)}}};
}

template <class Info>
Expression<Info> named_number_(std::string n) {
  return {{{named_number<Info>(get_as_char(n))}}};
}

Expression<Fock1DInfo> create_fock_op(short int x_coordinate, Type type,
                                      std::string symbol) {
  if (not isFockOpType(type)) {
    throw std::logic_error(
        "Can only use Fock op types for this creation function");
  }

  return {{{Operator<Fock1DInfo>(
      ordering_value(0),
      Fock1DInfo(x_coordinate, 0, type, get_as_char(symbol)))}}};
}

PyType create_normalised_ops(unsigned n, short int x, std::string sym) {
  return normalised_n_occupied_ops<Fock1DInfo>(n, x, get_as_char(sym));
}

Expression<Fock1DInfo> create_state_vector(short int x, short int state,
                                           Type type) {
  if (not isVectorType(type)) {
    throw std::logic_error(
        "Can only use vector types for this create function");
  }
  auto order = ordering_value(type == Type::STATE_VECTOR ? 1000 : -1000);
  return {{{Operator<Fock1DInfo>(order, Fock1DInfo(x, state, type, 'v'))}}};
}

Expression<Fock1DInfo> create_vacuum_state() {
  return create_state_vector(0, 0, Type::STATE_VECTOR);
}

Expression<Fock1DInfo> conjugate(Expression<Fock1DInfo> in) {
  return hermition_conjugate(in);
}

PyType normal_order_exp(PyType in, bool fermions) {
  auto sort_using =
      fermions ? SortUsing::ANTICOMMUTATORS : SortUsing::COMMUTATORS;
  auto res = normal_order(in);
  if (fermions) {
    return evaluate<Fock1DInfo>(res, boson_commutator<Fock1DInfo>,
                                default_fermion_subs<Fock1DInfo>, sort_using);
  }
  return evaluate<Fock1DInfo>(res, boson_commutator<Fock1DInfo>,
                              default_boson_subs<Fock1DInfo>, sort_using);
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
  handle.def("n_normalised_fock_ops", &create_normalised_ops);
  handle.def("state_vector", &create_state_vector);
  handle.def("vacuum_state", &create_vacuum_state);
  handle.def("hermitian_conjugate", &conjugate);
  handle.def("normal_order", &normal_order_exp);

  // Think maybe should use factories instead of constructors to help create
  // numbers
}

} // namespace operators
