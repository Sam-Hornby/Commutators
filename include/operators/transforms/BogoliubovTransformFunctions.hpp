#pragma once

#include "BogoliubovTransform.hpp"
#include <Expression/Infos/GenericInfo.hpp>


namespace operators {

template <OperatorType op_type>
struct DefaultBogTransformFunctions {
  template <class T>
  static unsigned get_order(const Operator<T> &) { return 1U; }

  template <class T>
  static Type get_type(const T &info) {
    return info.type;
  } 
  
  // group terms that are same except minus sign
  static std::size_t get_group(const Fock1DInfo &op) {
    return std::abs(op.x_coordinate); 
  }

  static OperatorType get_op_type() {
    return op_type;
  }

  static unsigned get_index(const Fock1DInfo &op) {
    return static_cast<unsigned>(op.x_coordinate < 0);
  }

  template <class InfoA>
  static OperatorPairs<InfoA> transform_ops(OperatorPairs<InfoA> input) {
    throw std::logic_error("Not implemented");
    return OperatorPairs<InfoA>();
  }
};


typedef DefaultBogTransformFunctions<OperatorType::FERMION> FermionSignedGroupingTransform;
typedef DefaultBogTransformFunctions<OperatorType::BOSON> BosonSignedGroupingTransform;



// For testing only 
template <OperatorType op_type>
struct GenericInfoBogTransform : public DefaultBogTransformFunctions<op_type> {
  static std::size_t get_group(const GenericInfo &op) {
    unsigned id = std::stoul(op.name().substr(1, 1));
    assert(id != 0); // indexing starts at 1
    id = id - 1;
    return id / 2;
  }
  static unsigned get_index(const GenericInfo &op) {
    unsigned id = std::stoul(op.name().substr(1, 1));
    assert(id != 0); // indexing starts at 1
    id = id - 1;
    return id % 2;
  }
  static Type get_type(const GenericInfo &op) {
    if (op.name()[0] != 'c') {
      return Type::UNSPECIFIED;
    }
    if (op.name().back() == '!') {
      return Type::CREATION_OPERATOR;
    }
    return Type::ANIHILATION_OPERATOR;
  }
  static GenericInfo subOutC(GenericInfo other) {
    if (other.op_name[0] != 'c') {
      std::abort();
    }
    other.op_name[0] = 'b';
    return other;
  } 
  static OperatorPairs<GenericInfo> transform_ops(OperatorPairs<GenericInfo> input) {
    // dont copy this as not very general, just using for tests
    spdlog::trace("Transform opertor pairs begin");
    spdlog::trace("{} {} {} {}", input.creation_ops[0].name(), input.creation_ops[1].name(),
                                 input.anihilation_ops[0].name(), input.anihilation_ops[1].name());
    input.creation_ops[0].info() = subOutC(input.creation_ops[0].info()); 
    input.creation_ops[1].info() = subOutC(input.creation_ops[1].info());
    input.anihilation_ops[0].info() = subOutC(input.anihilation_ops[0].info()); 
    input.anihilation_ops[1].info() = subOutC(input.anihilation_ops[1].info()); 
    spdlog::trace("Transform opertor pairs end");
    return input;
  }
};

typedef GenericInfoBogTransform<OperatorType::FERMION> FermionTestTransform;
typedef GenericInfoBogTransform<OperatorType::BOSON> BosonTestTransform;
}
