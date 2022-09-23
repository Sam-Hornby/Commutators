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

struct Fock1DInfo : public Fock0DInfo {
  short int x_coordinate;
  // This field is only used for state not creation/anihilation ops
  short int state = 0;

  Fock1DInfo(int x_coordinate) : x_coordinate(x_coordinate) {}
  Fock1DInfo(int x_coordinate, Type type)
      : Fock0DInfo(type), x_coordinate(x_coordinate) {}
  Fock1DInfo(int x_coordinate, int state, Type type)
      : Fock0DInfo(type), x_coordinate(x_coordinate), state(state) {}
  Fock1DInfo(int x_coordinate, int state, Type type, char symbol)
      : Fock0DInfo(type, symbol), x_coordinate(x_coordinate), state(state) {}
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
    } else if (type == Type::SPIN_OPERATORS) {
      return std::string("S") + std::string(1, symbol) +
                "_" + std::to_string(x_coordinate);
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
    if (x_coordinate != other.x_coordinate or state != other.state) {
      return false;
    }
    return static_cast<const Fock0DInfo&>(*this).match(
            static_cast<const Fock0DInfo&>(other));
  }
};

} // namespace operators
