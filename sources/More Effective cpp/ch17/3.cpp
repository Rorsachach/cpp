#include <iostream>
#include <string>

int main() {
  std::string s1("Nancy");
  std::string s2("Clancy");

  std::string &rs = s1;
  std::string *ps = &s1;

  ps = &s2;
  std::cout << "s1: " << s1 << std::endl;
  std::cout << "s2: " << *ps << std::endl;
  rs = s2;
  std::cout << "s1: " << s1 << std::endl;

  return 0;
}