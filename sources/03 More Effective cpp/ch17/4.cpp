#include <iostream>

using namespace std;

class BST {
public:
  BST()
  friend ostream &operator<<(ostream &os, const BST &rb);

  int size = 10; 
  int a[10];
};

ostream &operator<<(ostream &os, const BST &rb) {
  for (int i = 0; i < rb.size; ++i) {
    os << rb.a[i];
  }
  os << endl;
}

class BalancedBST : public BST {
public:
  int size = 100;
  int a[100];
};

void printBSTArray(ostream &os, const BST array[], int numElements) {
  for (int i = 0; i < numElements; ++i) {
    os << array[i];
  }
}

int main() {
  BalancedBST bArray[10];
  printBSTArray(cout, bArray, 10);
}