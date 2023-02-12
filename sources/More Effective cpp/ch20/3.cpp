#include <iostream>
#include <cstddef>
#include <string>

using namespace std;

size_t countChar(const string &str, char ch) {
  size_t count = 0;
  for (char c : str) {
    if (c == ch) {
      ++count;
    }
  }

  return count;
}

int main() {
  char buffer[1024] = "hello world";
  char c = 'l';

  cout << countChar(buffer, c) << endl;
}