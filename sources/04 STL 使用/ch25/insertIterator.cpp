#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <algorithm>

using namespace std;

int main() {
  list<int> coll1;
  for (int i = 0; i < 9; ++i) {
    coll1.push_back(i);
  }

  vector<int> coll2;
  copy(coll1.begin(), coll1.end(), back_inserter(coll2));

  deque<int> coll3;
  copy(coll1.begin(), coll1.begin(), front_inserter(coll3));

  set<int> coll4;
  copy(coll1.begin(), coll1.end(), inserter(coll4, coll4.begin()));
}