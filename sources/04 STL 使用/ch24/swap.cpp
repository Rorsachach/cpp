#include <algorithm>

class MyContainer {
public:
  MyContainer(int nums) : numElems(nums) {
    if (nums == 0) elems = 0;
    else elems = new int[nums];
  }
  ~MyContainer() {
    delete[] elems;
  }

  void swap(MyContainer& x) {
    std::swap(elems, x.elems);
    std::swap(numElems, x.numElems);
  }
private:
  int* elems;
  int numElems;
};

inline void swap(MyContainer& c1, MyContainer& c2) {
  c1.swap(c2);
}

int main() {
  MyContainer a(12);
  MyContainer b(5);
  swap(a, b);
}