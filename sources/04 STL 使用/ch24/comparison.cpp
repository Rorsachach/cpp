#include <string>
#include <iostream>
#include <utility>

using namespace std::rel_ops;

class Rational {
public:
  Rational(int x, int y) : numerator(x), denominator(y) {}
  bool operator==(const Rational& rhs) const {
    return ((*this).numerator * rhs.denominator) == ((*this).denominator * rhs.numerator);
  }
  bool operator<(const Rational& rhs) const {
    return ((*this).numerator * rhs.denominator) < ((*this).denominator * rhs.numerator);
  }

  friend std::ostream& operator<<(std::ostream& os, const Rational& r);
private:
  int numerator;
  int denominator;
};

std::ostream& operator<<(std::ostream& os, const Rational& r) {
  os << r.numerator << "/" << r.denominator;
  return os;
}

int main() {
  Rational a(1, 2);
  Rational b(3, 4);

  std::cout << a << " == " << b << " is: " << (a == b ? "true" : "false") << std::endl;
  std::cout << a << " < " << b << " is: " << (a < b ? "true" : "false") << std::endl;
  std::cout << a << " != " << b << " is: " << (a != b ? "true" : "false") << std::endl;
  std::cout << a << " > " << b << " is: " << (a > b ? "true" : "false") << std::endl;
  std::cout << a << " >= " << b << " is: " << (a >= b ? "true" : "false") << std::endl;
  std::cout << a << " <= " << b << " is: " << (a <= b ? "true" : "false") << std::endl;
}