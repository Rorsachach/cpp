#include <iostream>

using namespace std;

class Rational {
public:
  Rational(int numerator = 0, int denominator = 1) : _numerator(numerator),
    _denominator(denominator) {}
  
  // operator double() const {
  //   return static_cast<double>(_numerator) / static_cast<double>(_denominator);
  // }

  friend ostream &operator<<(ostream &os, const Rational &r);
  double asDouble() const {
    return static_cast<double>(_numerator) / static_cast<double>(_denominator);
  }

private:
  int _numerator;
  int _denominator;
};

ostream &operator<<(ostream &os, const Rational &r) {
  os << r._numerator << "/" << r._denominator;
  return os;
}

int main() {
  Rational r(1, 3);
  cout << r << endl;
  cout << r.asDouble() << endl;
}