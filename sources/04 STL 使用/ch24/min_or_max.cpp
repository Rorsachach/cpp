#include <algorithm>
#include <iostream>

bool int_ptr_less(int* a, int* b) {
  return *a < *b;
}

int main() {
  int* pa = new int(12);
  int* pb = new int(13);

  int* pmax = std::max(pa, pb, [](int* a, int* b) {
    return *a < *b;
  });
  // int* pmax = std::max(pa, pb, int_ptr_less);

  std::cout << "min(0, 1): " << std::min(0, 1) << std::endl;
  std::cout << "max(" << *pa << ", " << *pb << "): " << *pmax << std::endl;
}