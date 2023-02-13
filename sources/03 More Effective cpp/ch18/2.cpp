#include<iostream>

using namespace std;

class UPInt {
public:
  UPInt(int value) : _value(value) {}
  UPInt &operator++() {
    (*this)._value += 1;
    return *this;
  }
  const UPInt operator++(int) {
    UPInt oldValue = *this;
    ++(*this);
    return oldValue;
  }
  UPInt &operator--() {
    (*this)._value -= 1;
    return *this;
  }
  const UPInt operator--(int) {
    UPInt oldValue = *this;
    --(*this);
    return oldValue;
  }
  friend ostream &operator<<(ostream &os, UPInt);
private:
  int _value;
};

ostream &operator<<(ostream &os, UPInt upi) {
  os << "value: " << upi._value;
  return os;
}

int main() {
  UPInt upi(12);
  cout << upi++ << endl;
  cout << ++upi << endl;
  cout << upi-- << endl;
  cout << --upi << endl;

  return 0;
}