#pragma once
#include "Fock0D.hpp"
#include <stdexcept>
#include <tuple>

namespace operators {

/*
N = a!a

bosons:
Na! = a! * a * a! = a! (a a!) + a!(a! a - a!a)
                  = a! (a! a + a a! - a!a)
                  = a!N + a!(a a! - a!a)
                  = a!N + a![a, a!]

[N,a!] = [a, a!]

General:
{A, B} = AB + BA
AB = -BA + {A, B}

Fermions:
Na! = a!(a a!) = a!(-a! a + {a, a!})
               = -a! N + a!

number op has same {anti}-comutator relations as fock ops

*/

struct Fock1DInfo {
  short int x_coordinate;
  short int state =
      0; // This field is only used for state not creation/anihilation ops
  Type type = Type::UNSPECIFIED;
  // TODO make const
  char symbol = 'a';

  Fock1DInfo(int x_coordinate) : x_coordinate(x_coordinate) {}
  Fock1DInfo(int x_coordinate, Type type)
      : x_coordinate(x_coordinate), type(type) {}
  Fock1DInfo(int x_coordinate, int state, Type type)
      : x_coordinate(x_coordinate), state(state), type(type) {}
  Fock1DInfo(int x_coordinate, int state, Type type, char symbol)
      : x_coordinate(x_coordinate), state(state), type(type), symbol(symbol) {}
  Fock1DInfo(int x_coordinate, char symbol, Type type)
      : Fock1DInfo(x_coordinate, 0, type, symbol) {}
  Fock1DInfo() = default;

  std::string name() const {
    std::string result;
    if (isFockOpType(type)) {
      result += (symbol + std::string("_") + std::to_string(x_coordinate));
      if (type == Type::CREATION_OPERATOR) {
        result += "!";
      }
    } else if (isVectorType(type)) {
      result += (type == Type::STATE_VECTOR ? "|" : "<");
      if (x_coordinate == 0 and state == 0) {
        result += "0";
      } else {
        result += (std::to_string(x_coordinate) + "," + std::to_string(state));
      }
      result += (type == Type::STATE_VECTOR ? ">" : "|");
    } else {
      throw std::logic_error("No naming convention for unspecified");
    }
    return result;
  }

  bool operator==(Fock1DInfo other) const {
    return x_coordinate == other.x_coordinate and state == other.state &&
           type == other.type;
  }

  bool operator!=(Fock1DInfo other) const { return not(*this == other); }
  bool operator<(Fock1DInfo other) const {
    return std::tie(x_coordinate, state, type) <
           std::tie(other.x_coordinate, other.state, other.type);
  }
  static Fock1DInfo vacuumState() {
    return Fock1DInfo(0, 0, Type::STATE_VECTOR);
  }
  bool isVacuumState() const {
    return type == Type::STATE_VECTOR and state == 0;
  }
  bool isHCVacuumState() const {
    return type == Type::HC_STATE_VECTOR and state == 0;
  }
  bool match(const Fock1DInfo &other) const {
    if (isFockOpType(type) and isFockOpType(other.type)) {
      return x_coordinate == other.x_coordinate;
    }
    if (isVectorType(type) and isVectorType(other.type)) {
      return state == other.state and x_coordinate == other.x_coordinate;
    }
    if (type == other.type and type == Type::SPIN_OPERATORS) {
      return x_coordinate == other.x_coordinate;
    }
    return false;
  }
  Type get_type() const { return type; }

  void complex_conjugate() {
    this->type = Fock0DInfo::conjugate_type(this->type);
  }
};

} // namespace operators
