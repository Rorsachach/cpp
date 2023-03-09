#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

int main() {
  vector<int> coll;

  for (int i=0; i <= 9; ++i) {
    coll.push_back(i);
  }

  copy(coll.rbegin(), coll.rend(), 
      ostream_iterator<int>(cout, " "));
  cout << endl;
}