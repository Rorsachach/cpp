#include <bits/stdc++.h>

template<typename Key, typename Value, typename Compare = std::less<Key>()>
class AVL {
public:
  AVL() : size(0), root(0), compare(Compare()) {}

  struct Node {
    Node(int key, int value) : key(key), value(value) {}
    int key;
    int value;
    Node* left;
    Node* right;
    int height;
  };

  Node* insert(const Key& key, const Value& value, Node* root);

  Node* root;
  int size;
  Compare compare;
};

int getHeight(Node)

template<typename Key, typename Value, typename Compare>
AVL<Key, Value, Compare>::Node* AVL<Key, Value, Compare>::insert(const Key& key, const Value& value, Node* node) {
  if (node == nullptr) {
    ++size;
    return new Node(key, value);
  }

  if (compare(key, node->key)) node->left = insert(key, value, node->left);
  else if (key != node->key) node->right = insert(key, value, node->right);
  else node->value = value;

  node->height = 1 + std::max()
}