#pragma once

#include <Expression/Operator.hpp>

namespace operators {

enum class OperatorType {
  FERMION,
  BOSON,
};

template <class InfoA>
struct OperatorPairs {
  Operator<InfoA> creation_ops[2];
  Operator<InfoA> anihilation_ops[2];
};

template <class Info>
struct BogTransformFunctionsBase {
  OperatorType op_type;
  virtual OperatorType get_op_type() const { return op_type; }
  virtual unsigned get_order(const Operator<Info> &) const = 0;
  virtual Type get_type(const Info &) const = 0;
  virtual std::optional<std::size_t> get_group(const Info &) const = 0;
  virtual unsigned get_index(const Info &) const = 0;
  virtual OperatorPairs<Info>
  transform_ops(OperatorPairs<Info> input) const = 0;
  BogTransformFunctionsBase(OperatorType op_type) : op_type(op_type) {}
};

template <class Info>
struct PassOnTransforms : public BogTransformFunctionsBase<Info> {
  const BogTransformFunctionsBase<Info> &orig;
  unsigned get_index(const Info &t) const override { return orig.get_index(t); }
  unsigned get_order(const Operator<Info> &t) const override {
    return orig.get_order(t);
  }
  Type get_type(const Info &t) const override { return orig.get_type(t); }
  std::optional<std::size_t> get_group(const Info &t) const override {
    return orig.get_group(t);
  }
  OperatorPairs<Info> transform_ops(OperatorPairs<Info> input) const override {
    return orig.transform_ops(input);
  }
  OperatorType get_op_type() const override { return orig.get_op_type(); }

  PassOnTransforms(const BogTransformFunctionsBase<Info> &orig)
      : BogTransformFunctionsBase<Info>(orig.get_op_type()), orig(orig) {}
};

template <class Info>
struct SwapIndices : public PassOnTransforms<Info> {
  unsigned get_index(const Info &t) const override {
    unsigned index = this->orig.get_index(t);
    assert(index == 0 or index == 1);
    return index xor 1U;
  }
  using PassOnTransforms<Info>::PassOnTransforms;
};

} // namespace operators
