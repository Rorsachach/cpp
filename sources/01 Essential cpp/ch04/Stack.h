#include <vector>
#include <string>

class Stack
{
private:
    std::vector<std::string> _stack;
public:
    bool push(const std::string&elem);
    bool pop(std::string &elem);
    bool peek(std::string &elem);
    bool find(const std::string &elem);
    int count(const std::string &elem);

    bool empty();
    bool full();

    int size() {return _stack.size();}
};

inline bool Stack::empty() {
    return _stack.empty();
}

inline bool Stack::full() {
    return _stack.size() == _stack.max_size();
}