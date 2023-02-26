#include <iostream>

template<typename T>
class Rational {
public:
    Rational (const T& numerator = 0, const T& denominator = 1) : _numerator(numerator),
      _denominator(denominator) {}
    const T numerator() const { return _numerator; }
    const T denominator() const { return _denominator; }
    friend const Rational operator*(const Rational& lhs, const Rational& rhs) {
      return Rational(
        lhs.numerator() * rhs.numerator(),
        lhs.denominator() * rhs.denominator()
      );
    }
private:
    T _numerator;
    T _denominator;
};

int main() {
  Rational<int> oneHalf(1, 2);
  Rational<int> result;
  result = oneHalf * 2;
  result = 2 * oneHalf;

  std::cout << "result: " << result.numerator() << "/"
    << result.denominator() << std::endl;
  
  // result.operator*(oneHalf, 2); // 错误行为

  return 0;
}