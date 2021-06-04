#pragma once

#include <transforms/BogoliubovTransformFunctionsBase.hpp>
#include <Expression/Infos/GenericInfo.hpp>
#include "Logging.hpp"


namespace operators {

struct GroupAndIndex {
  std::size_t group;
  unsigned index;

  bool operator== (const GroupAndIndex &other) const {
    return group == other.group && index == other.index;
  }
  bool operator!= (const GroupAndIndex &other) const {
    return not (*this == other);
  }
};

template <class Info>
struct FindGroupsTransformFunctions : public PassOnTransforms<Info> {
  std::map<Info, GroupAndIndex> groupMap;
 
  boost::optional<std::size_t> get_group(const Info &op) const override {
    const auto it = groupMap.find(op);
    if (it == groupMap.end()) {
      return boost::none;
    }
    return it->second.group;
  }

  unsigned get_index(const Info &op) const override {
    return groupMap.at(op).index;
  }

  FindGroupsTransformFunctions(const BogTransformFunctionsBase<Info> &orig,
                               std::map<Info, GroupAndIndex> groupMap) :
      PassOnTransforms<Info>(orig), groupMap(std::move(groupMap)) {}
  
};  

namespace {

template <class Info>
Type default_get_type(const Info &info) {
  return info.get_type();
}

template <>
Type default_get_type<GenericInfo>(const GenericInfo &info) {
  const auto last_index = info.op_name.size() - 1;
  if (info.op_name[0] == '|' and info.op_name[last_index] == '>') {
    return Type::STATE_VECTOR;
  }
  if (info.op_name[0] == '<' and info.op_name[last_index] == '|') {
    return Type::HC_STATE_VECTOR;
  }
  if (info.op_name[last_index] == '!') {
    return Type::CREATION_OPERATOR;
  }
  return Type::ANIHILATION_OPERATOR; 
}

char swap_char(char in) {
  if (in != 'b') {
    return 'b';
  }
  return 'c';
}

template <class Info>
Info default_transform(Info in) {
  in.symbol = swap_char(in.symbol);
  return in;
}

template <>
GenericInfo default_transform(GenericInfo in) {
  in.op_name[0] = swap_char(in.op_name[0]);
  return in;
}

}

template <class Info>
struct DefaultBogTransformFunctions : public BogTransformFunctionsBase<Info> {
  std::map<Info, GroupAndIndex> groupMap;

  unsigned get_order(const Operator<Info> &) const override {
    return 1U;
  }
  Type get_type(const Info &info) const override {
    return default_get_type(info);
  }
  boost::optional<std::size_t> get_group(const Info &op) const override {
    std::abort();
  }
  unsigned get_index(const Info &op) const override {
    std::abort();
  }
  OperatorPairs<Info> transform_ops(OperatorPairs<Info> input) const override {
    input.creation_ops[0].info() = default_transform(input.creation_ops[0].info());
    input.creation_ops[1].info() = default_transform(input.creation_ops[1].info());
    input.anihilation_ops[0].info() = default_transform(input.anihilation_ops[0].info());
    input.anihilation_ops[1].info() = default_transform(input.anihilation_ops[1].info());
    return input;
  }
  DefaultBogTransformFunctions(OperatorType op_type) : BogTransformFunctionsBase<Info>(op_type) {
    this->op_type = op_type;
  }
};

template <class Info>
auto FermionDefaultTransforms() {
  return DefaultBogTransformFunctions<Info>(OperatorType::FERMION);
}
template <class Info>
auto BosonDefaultTransforms() {
  return DefaultBogTransformFunctions<Info>(OperatorType::BOSON);
}



// For testing only 
template <OperatorType op_type_test>
struct GenericInfoBogTransform : public DefaultBogTransformFunctions<GenericInfo> {
  boost::optional<std::size_t> get_group(const GenericInfo &op) const override {
    unsigned id = std::stoul(op.name().substr(1, 1));
    assert(id != 0); // indexing starts at 1
    id = id - 1;
    return id / 2;
  }
  unsigned get_index(const GenericInfo &op) const override {
    unsigned id = std::stoul(op.name().substr(1, 1));
    assert(id != 0); // indexing starts at 1
    id = id - 1;
    return id % 2;
  }
  Type get_type(const GenericInfo &op) const override {
    if (op.name()[0] != 'c') {
      return Type::UNSPECIFIED;
    }
    if (op.name().back() == '!') {
      return Type::CREATION_OPERATOR;
    }
    return Type::ANIHILATION_OPERATOR;
  }
  GenericInfo subOutC(GenericInfo other) const {
    if (other.op_name[0] != 'c') {
      std::abort();
    }
    other.op_name[0] = 'b';
    return other;
  } 
  OperatorPairs<GenericInfo> transform_ops(OperatorPairs<GenericInfo> input) const override {
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
  GenericInfoBogTransform() : DefaultBogTransformFunctions<GenericInfo>(op_type_test) {
    spdlog::info("GenereicTrans construct op {}", op_type_test);
    this->op_type = op_type_test;
  }

};

typedef GenericInfoBogTransform<OperatorType::FERMION> FermionTestTransform;
typedef GenericInfoBogTransform<OperatorType::BOSON> BosonTestTransform;
}
