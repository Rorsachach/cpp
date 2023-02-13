#include <string>

using namespace std;

int main()
{
  string &rc; // ! 错误，需要设定初值
  string s("hello");
  string &rc = s; // * 正确
}