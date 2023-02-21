#include <vector>

class WidgetImpl {
public:
private:
  int a, b, c;
  std::vector<double> v;
};

class Widget {
public:
  Widget(const Widget& rhs);
  Widget& operator=(const Widget& rhs) {
    *ipml = *(rhs.ipml);
  }
  void swap(Widget& other) {
    using std::swap;
    swap(ipml, other.ipml);
  }

private:
  WidgetImpl* ipml;
};

namespace std {
  template <>
  void swap<Widget>(Widget& a, Widget& b) {
    a.swap(b);
  }
}