#include <iostream>

using namespace std;

class Widget {
public:
  int a;
};

class SpecialWidget : public Widget {
public:
  int b;
};

void func () {
  SpecialWidget w;
  w.a = 1;
  w.b = 2;
  throw w;
}

int main () {
  try {
    func();
  }
  catch(Widget w) {
    SpecialWidget *sw = (SpecialWidget *) &w;
    cout << sw->b << endl;
  }
}