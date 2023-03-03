#include <iostream>
#include <string>

int main() {
  std::string s1 = "hello";
  char* p = &s1[1];
  std::string s2(s1);
  *p = 'x';
  std::cout << s2 << std::endl;
}