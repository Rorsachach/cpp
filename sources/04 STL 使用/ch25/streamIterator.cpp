#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>

using namespace std;

int main() {
  vector<string> coll;
  ifstream ifile;
  ifile.open("./build/ch25/test.txt");

  istream_iterator<string> beg(ifile);
  istream_iterator<string> end;

  copy(beg, end, back_inserter(coll));

  sort(coll.begin(), coll.end());

  unique_copy(coll.begin(), coll.end(), 
    ostream_iterator<string>(cout, " "));
  
  cout << endl;
}