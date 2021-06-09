import pytest
from FockModule import FockOperators


def test_constructing():
  creation = FockOperators.FockType(2)
  hc_vector = FockOperators.FockType(1)
  assert str(creation) == "FockType.CREATION_OPERATOR"

  assert float(str(FockOperators.number(7))) == 7
  assert str(FockOperators.named_number("z") == "z")
  d = FockOperators.fock_op(2, creation, "c");
  e = FockOperators.state_vector(2, 0, hc_vector)

  f = e + d
  g = f * f
  assert str(g) == "(<2,0| * <2,0|) + (c_2! * <2,0|) + (<2,0| * c_2!) + (c_2! * c_2!)"

  assert str(e - d) == "(<2,0|) + (-1.000000 * c_2!)"

  v = FockOperators.vacuum_state()
  assert str(v) == "|0>"
  assert str(FockOperators.hermitian_conjugate(v)) == "<0|"
  assert str(FockOperators.hermitian_conjugate(d)) == "c_2"
  assert str(FockOperators.hermitian_conjugate(
               FockOperators.hermitian_conjugate(d))) == str(d)

  
