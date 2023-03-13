#include <limits>
#include <string>
#include <iostream>
#include <climits>

void print(std::string s, auto a) {
  if (typeid(a) == typeid(bool)) {
    if (a == false) std::cout << s << ": false" << std::endl;
    else std::cout << s << ": true" << std::endl;
  } else {
    std::cout << s << ": " << a << std::endl;
  }
}

int main() {
  print("int 是否有极值", std::numeric_limits<int>::is_specialized);
  print("int 带有正负号", std::numeric_limits<int>::is_signed);
  print("int 是否是整数类别", std::numeric_limits<int>::is_integer);
  print("int 是否是精确值", std::numeric_limits<int>::is_exact);
  print("int 数值集合有限", std::numeric_limits<int>::is_bounded);
  print("int 正值相加可能因为溢出而回绕", std::numeric_limits<int>::is_modulo);
  print("int 最小值", std::numeric_limits<int>::min());
  print("int 最大值", std::numeric_limits<int>::max());
  print("int 位个数", std::numeric_limits<int>::is_specialized);
  print("int 底数", std::numeric_limits<int>::radix);
  print("int 底数的最小负整数指数", std::numeric_limits<int>::min_exponent);
  print("int 底数的最大负整数指数", std::numeric_limits<int>::max_exponent);
  print("int 最小间隔", std::numeric_limits<int>::epsilon());
  print("int 舍入风格", std::numeric_limits<int>::round_style);
  print("int 舍入误差", std::numeric_limits<int>::round_error());
  print("int 有正无穷大", std::numeric_limits<int>::has_infinity);
  print("int 正无穷大", std::numeric_limits<int>::infinity());
}