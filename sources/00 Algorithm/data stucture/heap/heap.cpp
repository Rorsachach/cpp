#include <iostream>
#include <vector>

class Heap {
public:
  Heap(std::vector<int>& in) : data(in) {
    make_heap();
  }

  void push(int val);
  int pop();
  bool empty() { return data.empty(); }

private:
  void up(int index);
  void down(int index);
  void make_heap();

private:
  std::vector<int> data;
};

void Heap::up(int index) {
  while (index > 1 && data[index - 1] > data[index / 2 - 1]) {
    std::swap(data[index - 1], data[index / 2 - 1]);
    index /= 2;
  }
}

void Heap::down(int index) {
  while (index * 2 <= data.size()) {
    int tmp = index * 2;
    if (tmp + 1 <= data.size() && data[tmp - 1] < data[tmp]) ++tmp;
    if (data[tmp - 1] < data[index - 1]) break;
    std::swap(data[tmp - 1], data[index - 1]);
    index = tmp;
  }
}

void Heap::push(int val) {
  data.push_back(val);
  up(data.size());
}

int Heap::pop() {
  std::swap(*data.begin(), *(data.end() - 1));
  int val = data.back();
  data.pop_back();
  down(1);

  return val;
}

void Heap::make_heap() {
  // for (int i = 1; i <= data.size(); ++i) 
  //   up(i);
  for (int i = data.size(); i > 0; --i)
    down(i);
}


int main() {
  std::vector<int> data{1,2,3,4,5,6,7,8,9,10};
  Heap h(data);
  while (!h.empty()) {
    int val = h.pop();
    std::cout << val << std::endl;
  }

  return 0;
}