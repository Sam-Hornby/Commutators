#ifndef _substitutions_hpp_
#define _substitutions_hpp_

#include "Operator.hpp"
#include "Utils.hpp"
#include <vector>

namespace operators {

// substitution functions take an iterator and the vector, the it is where to start looking for pattern from and the
// vector gives the end of the sequence and if subsituting in a longer expression then for insertions.
// If they find a pattern they must replace terms with new terms and insert any extra.
// If they find a pattern they must also return true, otherwise return false


// When replacement expression is smaller than what is being substituted is useful to set excess operators to 1 and then
// after the substitution phase call simplify numbers to remove them
void set_to_one(std::vector<Operator>::iterator it, const std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) {
    *(it + i) = Operator(1.0);
  }
}


// if every state in the expression is orthognal (and you are not generating any new ones) can zero all inner products
// between different states and set to one inner product of identical states. Assumes that the value of the operator
// info is used to show the state
bool all_states_orthognal(std::vector<Operator>::iterator start, std::vector<Operator> & exp) {
  if (is_hc_state_vector(*start)) {
    const auto value = start->info.value;
    if ((start + 1) != exp.end() and is_state_vector(*(start + 1))) {
      if ((start + 1)->info.value == value) {
        *start = Operator(1);
        set_to_one(start + 1, 1);
      } else {
        *start = Operator(0);
        set_to_one(start + 1, 1);
      }
      return true;
    }
  }
  return false;
}

// assumes the the value of operator_info for the vacuum state and its hc is 0
bool anihilate_vacuum(std::vector<Operator>::iterator start, std::vector<Operator> & exp) {
  if (start + 1 == exp.end()) {
    return false;
  }
  bool anihilate_hc = is_hc_state_vector(*start) and start->info.value == 0 and is_creation_op(*(start + 1));
  bool anihilate_st = is_anihilation_op(*start) and is_state_vector(*(start + 1)) and (start + 1)->info.value == 0;
  if (anihilate_hc or anihilate_st) {
    *start = Operator(0);
    set_to_one(start + 1, 1);
    return true;
  }
  return false;
}

bool no_subs(std::vector<Operator>::iterator, std::vector<Operator> &) {
  return false;
}

bool fermion_dual_occupation(std::vector<Operator>::iterator start, std::vector<Operator> & exp) {
  // for fermions a * a = 0 and a! * a! = 0 (from fact anti comutator is zero)
  if (start + 1 == exp.end()) {
    return false;
  }
  bool ani = is_anihilation_op(*start) and is_anihilation_op(*(start + 1));
  bool cre = is_creation_op(*start) and is_creation_op(*(start + 1));
  if ((ani or cre) and start->info.value == (start+1)->info.value) {
    *start = Operator(0);
    set_to_one(start + 1, 1);
    return true;
  }
  return false;
}

} // end name space


#endif
