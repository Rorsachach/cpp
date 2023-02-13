#include <iostream>

void printDouble(const double &rd)
{
  std::cout << rd << std::endl;
}

void printDouble(const double *pd)
{
  if (pd)
    std::cout << *pd << std::endl;
  else
    std::cout << "pd is nullptr" << std::endl;
}

int main() {
  double a = 2;
  printDouble(a);
  printDouble(&a);
  printDouble(nullptr);
}