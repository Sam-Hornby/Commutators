#pragma once

#include <complex>
#include <tuple>


namespace operators {

struct ImaginaryNumber {
  double value;
  explicit ImaginaryNumber(double val) : value(val) {}
};

struct ComplexNumber {
  std::complex<double> value = std::complex<double>(0, 0);
  ComplexNumber() = default;
  ComplexNumber(double a) : ComplexNumber(a, 0) {}
  ComplexNumber(ImaginaryNumber a) : ComplexNumber(0, a.value) {}
  ComplexNumber(double a, ImaginaryNumber b) : ComplexNumber(a, b.value) {}
  ComplexNumber(double r, double im) : value(r, im) {}
  ComplexNumber(std::complex<double> val) : value(val) {}
  std::string name() const {
    if (value.imag() == 0) {
      return std::to_string(value.real());
    }
    if (value.real() == 0) {
      return std::to_string(value.imag()) + " * i";
    }
    return "(" + std::to_string(value.real()) + " + " + std::to_string(value.imag()) + "i)";
  }
};


bool operator==(const ComplexNumber &A, const ComplexNumber &other) {
  return A.value == other.value;
}
bool operator!=(const ComplexNumber &A, const ComplexNumber &other) {
  return not (A == other);
}

bool operator<(const ComplexNumber &A, const ComplexNumber &other) {
  return std::make_tuple(A.value.real(), A.value.imag()) < std::make_tuple(other.value.real(), other.value.imag());
}

ImaginaryNumber operator*(ImaginaryNumber im, double r) {
  return ImaginaryNumber(im.value * r);
}

double operator*(ImaginaryNumber A, ImaginaryNumber B) {
  return A.value * B.value * -1;
}
ComplexNumber operator*(ComplexNumber A, ComplexNumber B) {
  return ComplexNumber(A.value * B.value);
}
ComplexNumber operator*(ImaginaryNumber A, ComplexNumber B) {
  return ComplexNumber(A) * B;
}
ComplexNumber operator*(double A, ComplexNumber B) {
  return ComplexNumber(A) * B;
}

ComplexNumber operator+(ComplexNumber A, ComplexNumber B) {
  return ComplexNumber(A.value + B.value);
}
ComplexNumber operator+(ImaginaryNumber A, ComplexNumber B) {
  return ComplexNumber(A) + B;
}
ComplexNumber operator+(double A, ComplexNumber B) {
  return ComplexNumber(A) + B;
}

}
