#include <iostream>
#include <algorithm>
#include "Stack.h"

bool Stack::push(const std::string &elem) {
    if (full())
        return false;

    _stack.push_back(elem);
    return true;
}

bool Stack::pop(std::string &elem) {
    if (empty())
        return false;
    
    elem = _stack.back();
    _stack.pop_back();
    return true;
}

bool Stack::peek(std::string &elem) {
    if (empty())
        return false;
    
    elem = _stack.back();
    return true;
}

bool Stack::find(const std::string &elem) {
    std::vector<std::string>::iterator it;
    it = std::find(_stack.begin(), _stack.end(), elem);
    return it != _stack.end();
}

int Stack::count(const std::string &elem) {
    int cnt = 0;
    std::vector<std::string>::iterator it;
    for (it = _stack.begin(); it != _stack.end(); ++it) {
        if (*it == elem)
            cnt++;
    }

    return cnt;
}

void fill_stack(Stack &stack, std::istream &is = std::cin) {
    std::string str;
    while (is >> str && str != "quit" && !stack.full())
        stack.push(str);
}

void walk_stack(Stack &stack) {
    while (!stack.empty()) {
        std::string str;
        stack.pop(str);
        std::cout << str << std::endl;
    }
}

int main() {
    Stack stack;
    fill_stack(stack);
    std::string str;
    stack.peek(str);
    std::cout << "Top: " << str << std::endl;
    std::cout << "find 'std': " << stack.find("std") << std::endl;
    std::cout << "find 'hello': " << stack.find("hello") << std::endl;

    std::cout << "count 'hello': " << stack.count("hello") << std::endl;
    walk_stack(stack);
}