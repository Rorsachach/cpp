#include <iostream>
#include <string>

class Window {
public:
    std::string name() const {
        return _name;
    }
    virtual void display() const {
        std::cout << "Window" << std::endl;
    }

protected:
    std::string _name;
};

class WindowWithScrollBars : public Window {
public:
    virtual void display() const {
        std::cout << "WWSB" << std::endl;
    }
};

void printNameAndDisplay(const Window& w) {
    std::cout << w.name();
    w.display();
}

int main() {
    WindowWithScrollBars w;
    printNameAndDisplay(w);
}