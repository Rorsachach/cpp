#include <iostream>

class B {
public:
  virtual void f() const {
    std::cout << "i'm B" << std::endl;
  }
};

class D : public B {
public:
  virtual void f() {
    std::cout << "i'm D" << std::endl;
  }
};

int main() {
  B* pb = new D;
  pb->f();
}