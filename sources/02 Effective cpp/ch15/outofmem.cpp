#include <iostream>
#include <new>

void outOfMem() {
  std::cerr << "Unable to satisfy request for memory\n";
  // std::abort(); // 终结
}

int main() {
  std::set_new_handler(outOfMem);
  int* pBigDataArray = new int[10000000000L];
}