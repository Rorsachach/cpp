#include <vector>
#include <iostream>

int main() {
  std::vector<int> coll;
  for (int i = 0; i < 6; ++i) {
    coll.push_back(i);
  }

  for (int i = 0; i < coll.size(); ++i) {
    std::cout << coll[i] << " ";
  }

  std::cout << std::endl;
}