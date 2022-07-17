#pragma once
#include "Fock0D.hpp"
#include <absl/algorithm/algorithm.h>
#include <vector>

/*
Spin infos
3 operators Sx, Sy, Sz + 2 combo ops S+ = (Sx + iSy) and S- = (Sx - iSy)
Sx,Sy,Sz are hermitian ladder operators are not

common comutators are 
[Sx, Sy] = iSz | [Sy, Sz] = iSx | [Sz, Sx] = iSy
ladder commutators
[Sz, S+] = S+ | [Sz, S-] = -S- | [S+, S-] = 2Sz 

 this is only for 3D space
*/

namespace operators {

const std::vector<char> allowed_axis = {'x', 'y', 'z'};

Fock0DInfo create_spin_operator(char axis) {
  if (absl::c_none_of(allowed_axis, [&] (char c) {return c == axis;})) {
    throw std::logic_error(
        std::string("Axis must be one of x,y,z is: ")
        + std::to_string(axis));
  }
  Fock0DInfo result(Type::SPIN_OPERATORS);
  result.symbol = axis;
  return result;
}

}

