#include <iostream>

class Rational {
public:
    Rational (int numerator = 0, int denominator = 1) : _numerator(numerator),
      _denominator(denominator) {}
    int numerator() const { return _numerator; }
    int denominator() const { return _denominator; }

    // const Rational operator*(const Rational& rhs) const {
    //   return Rational((*this).numerator() * rhs.numerator(), 
    //     (*this).denominator() * rhs.denominator());
    // }

private:
    int _numerator;
    int _denominator;
};

const Rational operator*(const Rational& lhs, const Rational& rhs) {
  return Rational(lhs.numerator() * rhs.numerator(), 
    lhs.denominator() * rhs.denominator());
}

int main() {
  Rational oneHalf(1, 2);
  Rational result;
  result = oneHalf * 2;
  result = 2 * oneHalf;

  std::cout << "result: " << result.numerator() << "/"
    << result.denominator() << std::endl;
  
  return 0;
}