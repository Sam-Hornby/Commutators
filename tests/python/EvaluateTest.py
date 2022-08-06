from FockModule import FockOperators


def test_normal_order():
  creation = FockOperators.FockType(2)
  #a = FockOperators.fock_op(0, creation, "a")
  #a_d = FockOperators.hermitian_conjugate(a)
  #v = FockOperators.vacuum_state()
  #v_d = FockOperators.hermitian_conjugate(v)
  v = FockOperators.from_string("|0>")
  v_d = FockOperators.from_string("<0|")

  N = FockOperators.from_string("a! * a")
  N_d = FockOperators.from_string("a * a_d")
  V = N * v
  V2 = v_d * N * v
  #N = a_d * a
  #N_d = a * a_d
  #V = N * v
  #V2 = v_d * N * v


  ON = FockOperators.normal_order(N, False)
  ON_d = FockOperators.normal_order(N_d, False)
  OV = FockOperators.normal_order(V, False)
  OV2 = FockOperators.normal_order(V2, True)


  assert str(ON) == str(N_d + FockOperators.number(1))
  assert str(ON_d) == str(ON_d)
  assert str(OV) == str(FockOperators.vacuum_state())
  assert(str(OV2)) == str(FockOperators.number(1))

#def test_harmonic_osc():



if __name__ == "__main__":
  test_normal_order()
