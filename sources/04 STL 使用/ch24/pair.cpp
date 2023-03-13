#include <iostream>
#include <utility>

int main() {
  std::pair<int, int> a(1, 2);
  std::pair<int, int> b(2, 3);
  std::cout << (a == b) << std::endl;

  auto c = std::make_pair(3, "hello");
}