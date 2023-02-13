#include <iostream>

using namespace std;

template <typename elemType>
class BinaryTree;

template <typename valType>
class BTnode {
    friend class BinaryTree<valType>;
public:
    BTnode(const valType &val) : _val(val) {
        _cnt = 1;
        _left = _right = 0;
    }
private:
    valType _val;
    int _cnt;
    BTnode *_left;
    BTnode *_right;

    void insert_value(const valType &val);
    void remove_value(const valType &val, BTnode *& prev);
    void preorder(BTnode *pt, ostream &os) const;
};

template <typename elemType>
class BinaryTree {
public:
    BinaryTree() : _root(0) {}
    BinaryTree(const BinaryTree &);
    ~BinaryTree();
    BinaryTree& operator=(const BinaryTree &);

    bool empty() {return _root == 0; }
    void clear() {
        if (_root) {
            clear(_root);
            _root = 0;
        }
    }
    void insert(const elemType &elem);
    void remove(const elemType &elem);

    void preorder();
private:
    BTnode<elemType> *_root;
    void copy(BTnode<elemType> *tar, BTnode<elemType> *src);
    void remove_root();
    void clear(BTnode<elemType> *);
};