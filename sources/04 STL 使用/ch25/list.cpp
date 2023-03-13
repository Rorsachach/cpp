#include <list>
#include <iostream>

int main() {
  std::list<char> coll;

  for (char c = 'a'; c <= 'z'; ++c) {
    coll.push_back(c);
  }

  for (std::list<char>::const_iterator it = coll.begin(); it != coll.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  while (!coll.empty()) {
    std::cout << coll.front() << " ";
    coll.pop_front();
  }
  std::cout << std::endl;
}