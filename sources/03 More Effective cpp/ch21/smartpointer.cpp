#include <iostream>
#include <vector>


template <class T>
class SmartPointer {
public:
  SmartPointer(T* ptr = 0) : pointee(ptr) {}
  SmartPointer(SmartPointer<T>& rhs);
  SmartPointer<T>& operator=(SmartPointer<T>& rhs);
  ~SmartPointer() {
    if (pointee != 0) delete pointee;
  }

  T& operator*() const {
    return *pointee;
  }
  T* operator->() const {
    return pointee;
  }

  bool isNull() { return pointee == 0; }
  // operator void*() {
  //   return static_cast<void*>(pointee);
  // }
  bool operator!() const {
    return pointee == 0;
  }

private:
  T* pointee;
};

template <class T>
SmartPointer<T>::SmartPointer(SmartPointer<T>& rhs) {
  pointee = rhs.pointee;
  rhs.pointee = 0;
}

template <class T>
SmartPointer<T>& SmartPointer<T>::operator=(SmartPointer<T>& rhs) {
  if (this == &rhs) return *this;

  delete pointee;
  pointee = rhs.pointee;
  rhs.pointee = 0;

  return *this;
}

int main() {
  SmartPointer<std::vector<int>> pVec1(new std::vector<int>({1,2,3,4,5,6}));
  SmartPointer<std::vector<int>> pVec2;
  if (pVec2.isNull()) std::cout << "pVec2.isNull" << std::endl;
  // if (pVec2 == 0) std::cout << "pVec2 == 0" << std::endl;
  if (!pVec2) std::cout << "!pVec2" << std::endl;

  SmartPointer<std::vector<int>> pVec3(new std::vector<int>({1,2,3,4,5,6}));
  // if (pVec2 == pVec3) std::cout << std::endl;
  pVec2 = pVec1;
  // if (pVec1 == 0) std::cout << "pVec1 == 0" << std::endl;
}