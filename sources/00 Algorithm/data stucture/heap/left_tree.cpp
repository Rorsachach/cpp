#include <iostream>
#include <vector>

class LeftTree {
public:
  LeftTree(std::vector<int>& in) {
    make_heap();
  }

  void merge(const LeftTree& rhs);
  void push(int val);
  int pop();
  bool empty() const { return data.empty(); }

  class TreeNode {
  public:
    TreeNode(int val_, int dist_ = 1) : val(val_), dist(dist_) {}
    int val;
    int dist;
  };

private:
  void up(int index);
  void down(int index);
  void make_heap();

private:
  std::vector<TreeNode> data;
};

void LeftTree::merge(const LeftTree& rhs) {
  if (rhs.empty()) return;
  else if ((*this).empty()) {
    (*this).data = rhs.data;
    return;
  }
  LeftTree& lhs = *this;

  std::vector<TreeNode> data(lhs.data.size() + rhs.data.size());
}

void LeftTree::up(int index) {
  while (index > 1 && data[index - 1].val > data[index / 2 - 1].val) {
    std::swap(data[index - 1], data[index / 2 - 1]);
    index /= 2;
  }
}

void LeftTree::down(int index) {
  while (index * 2 <= data.size()) {
    int tmp = index * 2;
    if (tmp + 1 <= data.size() && data[tmp - 1] < data[tmp]) ++tmp;
    if (data[tmp - 1] < data[index - 1]) break;
    std::swap(data[tmp - 1], data[index - 1]);
    index = tmp;
  }
}

void LeftTree::push(int val) {
  TreeNode node(val);
  data.push_back(val);
  up(data.size());
}

int LeftTree::pop() {
  std::swap(*data.begin(), *(data.end() - 1));
  TreeNode node = data.back();
  data.pop_back();
  down(1);

  return node.val;
}

void LeftTree::make_heap() {
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