#pragma once

namespace operators {

struct ImaginaryNumber {
  double value;
  explicit ImaginaryNumber(double val) : value(val) {}
};

struct ComplexNumber {
  double real_part = 0;
  double imaginary_part = 0;
  ComplexNumber() = default;
  ComplexNumber(double a) : real_part(a) {}
  ComplexNumber(ImaginaryNumber a) : imaginary_part(a.value) {}
  ComplexNumber(double a, ImaginaryNumber b) : real_part(a), imaginary_part(b.value) {}
  ComplexNumber(double r, double im) : real_part(r), imaginary_part(im) {}
  std::string name() const {
    if (imaginary_part == 0) {
      return std::to_string(real_part);
    }
    if (real_part == 0) {
      return std::to_string(imaginary_part) + " * i";
    }
    return "(" + std::to_string(real_part) + " + " + std::to_string(imaginary_part) + "i)";
  }
};


bool operator==(const ComplexNumber &A, const ComplexNumber &other) {
  return std::tie(A.real_part, A.imaginary_part) == std::tie(other.real_part, other.imaginary_part);
}
bool operator!=(const ComplexNumber &A, const ComplexNumber &other) {
  return not (A == other);
}

bool operator<(const ComplexNumber &A, const ComplexNumber &other) {
  return std::tie(A.real_part, A.imaginary_part) < std::tie(other.real_part, other.imaginary_part);
}


template <typename T>
ImaginaryNumber operator*(ImaginaryNumber im, T r) {
  static_assert(not std::is_same<T, ImaginaryNumber>::value, "Should use overload version");
  return ImaginaryNumber(im.value * r);
}
double operator*(ImaginaryNumber A, ImaginaryNumber B) {
  return A.value * B.value * -1;
}
ComplexNumber operator*(ComplexNumber A, ComplexNumber B) {
  return ComplexNumber((A.real_part * B.real_part) - (A.imaginary_part * B.imaginary_part),
                        (A.real_part * B.imaginary_part) + (A.imaginary_part * B.real_part));
}
ComplexNumber operator*(ImaginaryNumber A, ComplexNumber B) {
  return ComplexNumber(A) * B;
}
ComplexNumber operator*(double A, ComplexNumber B) {
  return ComplexNumber(A) * B;
}

ComplexNumber operator+(ComplexNumber A, ComplexNumber B) {
  return ComplexNumber(A.real_part + B.real_part, A.imaginary_part + B.imaginary_part);
}
ComplexNumber operator+(ImaginaryNumber A, ComplexNumber B) {
  return ComplexNumber(A) + B;
}
ComplexNumber operator+(double A, ComplexNumber B) {
  return ComplexNumber(A) + B;
}

}
