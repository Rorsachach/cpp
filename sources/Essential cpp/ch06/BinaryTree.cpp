#include "BinaryTree.h"
#include <string>
#include <iostream>

using namespace std;

template <typename elemType>
inline BinaryTree<elemType>::BinaryTree(const BinaryTree &rhs) {
    copy(_root, rhs._root);
}

template <typename elemType>
inline BinaryTree<elemType>::~BinaryTree() {
    clear();
}

template <typename elemType>
inline BinaryTree<elemType>& BinaryTree<elemType>::operator=(const BinaryTree &rhs) {
    if (this != &rhs) {
        clear();
        copy(_root, rhs._root);
    }

    return *this;
}

template <typename elemType>
void BinaryTree<elemType>::copy(BTnode<elemType> *tar, BTnode<elemType> *src) {
    if (src) {
        if (!tar) 
            tar = new BTnode<elemType>(src->_val);
        if (src->_left) {
            tar->_left = new BTnode<elemType>(src->_left->_val);
            copy(tar->_left, src->_left);
        }
        if (src->_right) {
            tar->_right = new BTnode<elemType>(src->_right->_val);
            copy(tar->_right, src->_right);
        }
    }
}

template <typename elemType>
inline void BinaryTree<elemType>::insert(const elemType &elem) {
    if (_root == 0) 
        _root = new BTnode<elemType>(elem);
    else
        _root->insert_value(elem);
}

template <typename elemType>
inline void BinaryTree<elemType>::remove(const elemType &elem) {
    if (_root) {
        if (_root->_val == elem)
            remove_root();
        else
            _root->remove_value(elem, _root);
    }
}

template <typename elemType>
inline void BinaryTree<elemType>::remove_root() {
    if (!_root) return;

    BTnode<elemType> *root = _root;
    if (!_root->_right) {
        _root = _root->_left;
    } else {
        _root = _root->_right;
        if (root->_left) {
            BTnode<elemType> *tmp = _root;
            while (tmp->_left) tmp = tmp->_left;
            tmp->_left = root->_left;
        }
    }
    delete root;
}

template <typename elemType>
void BinaryTree<elemType>::clear(BTnode<elemType> *pt) {
    if (pt) {
        clear(pt->_left);
        clear(pt->_right);
        delete pt;
    }
}

template <typename elemType>
void BinaryTree<elemType>::preorder() {
    if (_root) 
        _root->preorder(_root, cout);
}




template <typename valType>
void BTnode<valType>::insert_value(const valType &val) {
    if (val == _val) {
        _cnt++;
        return;
    }

    if (val < _val) {
        if (!_left) _left = new BTnode<valType>(val);
        else _left->insert_value(val);
    } else {
        if (!_right) _right = new BTnode<valType>(val);
        else _right->insert_value(val);
    }
}

template <typename valType>
void BTnode<valType>::remove_value(const valType &val, BTnode *& prev) {
    if (val < _val) {
        if (!_left) return;
        else _left->remove_value(val, _left);
    } else if (val > _val) {
        if (!_right) return;
        else _right->remove_value(val, _right);
    } else {
        if (_right) {
            prev = _right;
            if (_left) {
                BTnode<valType> *tmp = prev;
                while (tmp->_left) tmp = tmp->_left;
                tmp->_left = _left;
            }
        } else prev = _left;
        delete this;
    }
}

template <typename valType>
void BTnode<valType>::preorder(BTnode *pt, ostream &os) const {
    if (pt) {
        os << pt->_val << "  ";
        if (pt->_left) preorder(pt->_left, os);
        if (pt->_right) preorder(pt->_right, os);
    }
}

int main() {
    BinaryTree<string> bt;
    
    bt.insert("Piglet");
    bt.insert("Eeyore");
    bt.insert("Roo");
    bt.insert("Tigger");
    bt.insert("Chris");
    bt.insert("Pooh");
    bt.insert("Kanga");

    BinaryTree<string> bt2(bt);

    cout << "Preorder traversal: " << endl;
    bt.preorder(); 
    cout << "\n\n";

    bt.remove("Piglet");

    cout << "Preorder traversal: " << endl;
    bt.preorder(); 
    cout << "\n\n";

    bt.remove("Eeyore");

    cout << "Preorder traversal: " << endl;
    bt.preorder(); 
    cout << "\n\n";

    cout << "Preorder traversal: " << endl;
    bt2.preorder(); 
    cout << "\n\n";

    return 0;
}