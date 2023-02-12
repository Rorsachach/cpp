#include <cstring>

using namespace std;

/**
 * | 计数 | 字符串
 */
class String {
private:
  char *_pstr;

public:
  String() {
    _pstr = new char[5]() + 4;
    *(int *)(_pstr - 4) = 1; // 给计数部分赋初值
  }

  String (const char *pstr) {
    String();
    strcpy(_pstr, pstr);
  }

  String (const String &rs) : _pstr(rs._pstr) {
    ++*(int *)(_pstr - 4);
  }
};

