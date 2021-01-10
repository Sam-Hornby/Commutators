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
    std::logic_error("Not implemented");
    return OperatorPairs<InfoA>();
  }
};


typedef DefaultBogTransformFunctions<OperatorType::FERMION> FermionSignedGroupingTransform;
typedef DefaultBogTransformFunctions<OperatorType::BOSON> BosonSignedGroupingTransform;



// For testing only 
template <OperatorType op_type>
struct GenericInfoBogTransform : public DefaultBogTransformFunctions<op_type> {
  static std::size_t get_group(const GenericInfo &op) {
    // do stuff
    std::abort();
  }
  static unsigned get_index(const GenericInfo &op) {
    // do stuff
    std::abort();
  }
  static Type get_type(const GenericInfo &op) {
    // do stuff
    std::abort();
  }
};

typedef GenericInfoBogTransform<OperatorType::FERMION> FermionTestTransform;
typedef GenericInfoBogTransform<OperatorType::BOSON> BosonTestTransform;
}
