#include <string>

using namespace std;

class Triangular {
public:
    Triangular() {
        _length = 1;
        _beg_pos = 1;
        _next = 0;
    }
    Triangular(int len = 1) {Triangular(len, 1);}
    Triangular(int len = 1, int bp = 1) {
        _length = len > 0 ? len : 1;
        _beg_pos = bp > 0 ? bp : 1;
        _next - _beg_pos - 1;
    }
private:
    string _name;
    int _length;
    int _beg_pos;
    int _next;
}