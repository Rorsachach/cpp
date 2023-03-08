#include <iostream>
#include <deque>

int main() {
  std::deque<int> coll;

  for (int i = 0; i < 6; ++i) {
    coll.push_front(i);
  }

  while (!coll.empty()) {
    std::cout << coll.back() << " ";
    coll.pop_back();
  }

  std::cout << std::endl;
}