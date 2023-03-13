#include <iostream>
#include <memory>

//! 千万不要写成：std::ostream& operator<<(std::ostream& os, const std::auto_ptr<T> p)
template <class T>
std::ostream& operator<<(std::ostream& os, const std::auto_ptr<T>& p) {
  if (p.get() == nullptr) os << "null";
  else os << *p;
  return os;
}

int main() {
  std::auto_ptr<int> p(new int(42));
  std::auto_ptr<int> q;

  std::cout << "after initialization:" << std::endl;
  std::cout << "p: " << p << std::endl;
  std::cout << "q: " << q << std::endl;

  q = p;
  std::cout << "after assigning auto pointers:" << std::endl;
  std::cout << "p: " << p << std::endl;
  std::cout << "q: " << q << std::endl;

  *q += 13;
  p = q;
  std::cout << "after change and assigning:" << std::endl;
  std::cout << "p: " << p << std::endl;
  std::cout << "q: " << q << std::endl;

  std::cout << "================================" << std::endl;

  const std::auto_ptr<int> a(new int(20));
  const std::auto_ptr<int> b(new int(0));
  const std::auto_ptr<int> c;

  std::cout << "after initialization:" << std::endl;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;

  *b = *a;
  //! *c = *a; // 错误写法
  *a = -77;

  std::cout << "after assigning auto pointers:" << std::endl;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;

  //! a = b;
  //! c = b;
  std::auto_ptr<int>::element_type;

  std::cout << "================================" << std::endl;

  std::auto_ptr<int> e;
  e.reset(new int(12));
  std::cout << "after reset auto pointers:" << std::endl;
  std::cout << "e: " << e << std::endl;
}