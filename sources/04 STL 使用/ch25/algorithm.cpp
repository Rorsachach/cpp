#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

int main() {
  std::vector<int> coll;
  for (int i = 0; i < 10; ++i) {
    coll.push_back(std::rand() % 100);
  }

  std::cout << "coll is: ";
  for (int a : coll) {
    std::cout << a << " ";
  }
  std::cout << std::endl;

  std::vector<int>::iterator it;
  it = std::max_element(coll.begin(), coll.end());
  std::cout << "max element of coll is: " << *it << std::endl;
  it = std::min_element(coll.begin(), coll.end());
  std::cout << "min element of coll is: " << *it << std::endl;

  std::sort(coll.begin(), coll.end());
  std::cout << "sorted coll is: ";
  for (int a : coll) {
    std::cout << a << " ";
  }
  std::cout << std::endl;

  while ((it = std::find(coll.begin(), coll.end(), std::rand() % 100)) == coll.end());
  std::cout << "coll[" << std::distance(coll.begin(), it) << "] is " << *it << std::endl;

  std::reverse(it, coll.end());
  std::cout << "reversed coll (from [" << std::distance(coll.begin(), it) << "] to end) is: ";
  for (int a : coll) {
    std::cout << a << " ";
  }
  std::cout << std::endl;
}