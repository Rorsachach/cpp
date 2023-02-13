#include <iostream>

using namespace std;

int main() {
  // int i = 0;
  double i = 0.0;
  try {
    if (i == 0) {
      throw i;
    }
  }
  catch (double d) {
    cout << d << endl;
  }

  return 0;
}