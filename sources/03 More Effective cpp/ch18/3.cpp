#include <new>
#include <iostream>

class Widget {
public:
  Widget(int widgetSize) : _size(widgetSize) {}
  int size() {return _size;}
private:
  int _size;
};

int main() {
  void *rawMemory = operator new(sizeof(Widget));
  Widget *pw = static_cast<Widget *>(rawMemory);
  new (pw) Widget(12);
  std::cout << pw->size() << std::endl; 
  pw->~Widget();
  operator delete(rawMemory);
}