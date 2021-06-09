from FockModule import FockOperators


def test_normal_order():
  creation = FockOperators.FockType(2)
  a = FockOperators.fock_op(0, creation, "a")
  a_d = FockOperators.hermitian_conjugate(a)

  N = a_d * a
  N_d = a * a_d
  V = N * FockOperators.vacuum_state()

  ON = FockOperators.normal_order(N, False)
  ON_d = FockOperators.normal_order(N_d, False)
  OV = FockOperators.normal_order(V, False)

  assert str(ON) == str(N_d + FockOperators.number(1))
  assert str(ON_d) == str(ON_d)
  assert str(OV) == str(FockOperators.vacuum_state())


if __name__ == "__main__":
  test_normal_order()