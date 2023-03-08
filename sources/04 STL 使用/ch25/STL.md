# 标准模板库

## 1. STL 组件
- 容器 Container: 用来管理某类对象的集合。
- 迭代器 Iterator: 用来在一个对象群集的元素上进行遍历。
- 算法 Algorithm: 用来处理群集中的元素。

STL 的基本想法是将数据和操作分离，其中 容器来管理数据，算法对数据的操作进行定义，迭代器则是两
者之间数据传输的接口。

STL 的这种想法违背了 OOP 的思想，但是具有非常大的弹性。除此之外，STL 也是泛型编程的一个出色
样例。STL 通过提供 更泛型化的组件 来实现，即 适配器adapter 和 仿函数 functors。

## 2. 容器 container
容器用来管理一组元素，STL 提供了两种类型的 container：
- 序列式容器 Sequnence container：它管理的群集，每个元素有固定位置，这和插入顺序有关。主要包括： vector deque list
- 关联式容器 Associative containter：它管理的群集，每个元素也有固定位置，但是这和特定的排序规则相关，与插入顺序无关。主要包括：set multiset map multimap

虽然关联式容器会自动对元素进行排序，但是这并不意味着他们就是用来排序的。你也可以手动对序列式容
器进行排序。自动排序的优点是：寻找元素时效率更佳。

### 序列式容器
- vector

vector 将元素放入一个 dynamic array 中进行管理，允许随机存取。在 array 的尾部添加和删除元
素非常快速，但是在 array 中部和头部则非常耗时，因为需要对所有后续的元素进行前移或后移操作。

```cpp
#include <vector>
#include <iostream>

int main() {
  std::vector<int> coll;
  for (int i = 0; i < 6; ++i) {
    coll.push_back(i);
  }

  for (int i = 0; i < coll.size(); ++i) {
    std::cout << coll[i] << " ";
  }

  std::cout << std::endl;
}
```

- deque
deque 双向队列，一个 dynamic array，可以向两端发展，因此不论在头部还是尾部安插元素都十分迅
速。

```cpp
#include <iostream>
#include <deque>

int main() {
  std::deque<int> coll;

  for (int i = 0; i < 6; ++i) {
    coll.push_front(i);
  }

  while (!coll.empty()) {
    std::cout << coll.back() << " ";
    coll.pop_back();
  }

  std::cout << std::endl;
}
```

- list
双向链表，不提供随机存取，但是方便进行插入和删除操作。

```cpp
#include <list>
#include <iostream>

int main() {
  std::list<char> coll;

  for (char c = 'a'; c <= 'z'; ++c) {
    coll.push_back(c);
  }

  while (!coll.empty()) {
    std::cout << coll.front() << " ";
    coll.pop_front();
  }

  std::cout << std::endl;
}
```    

- string
string 也可以当作 STL 来使用，包括 basic_string<> string wstring。它们和 vector 很像，
只是元素为字符。

- array
array 并不是 STL 容器，他没有 STL 容器那么多通用接口函数。但是 STL 允许你对 array 调用 STL 
算法。

### 关联式容器
关联式容器依据特定的排序准则自动排序。通常是比较 key/value，缺省情况下使用 operator< 进行比
较。但是也可以提供自己的比较函数，定义出不同的排序准则。

通常关联容器使用二叉树来实现。不同关联式容器的区别在于元素的类型以及处理重复元素时的方式不一样。

- set: set 的内部元素依据值自动排序，每个元素只出现一次，不允许重复。
- multiset: 与 set 类似，但是允许重复元素
- map: 存储 key/value pair，每个 key 只能出现一次，可以被视为具有任意索引类型的数组。
- multimap: 与 map 类似，但是允许 key 重复。multimap 可以被当作字典使用

### 容器适配器 container adapter
除了上面的这些基本容器类型，STL 还提供了一些特别的 adapter，根据基本容器类型实现。

- stack: 对元素采用 LIFO 管理策略
- queue: 对元素才哟个 FIFO 策略，可以被看做一个普通缓冲区
- priority_queue: 有限队列，基于优先权进行排序

## 3. 迭代器 iterator
迭代器用来指出容器中的一个特定位置。其基本操作包括:
- operator*/operator->: 前者可以直接取值，后者可以取出成员
- operator++/operator--: 将迭代器前进/后退
- operator==/operator!=: 判断两个迭代器是否指向同一位置
- operator=: 为迭代器进行赋值操作

这些操作与操作 array 元素时的指针接口一致，不同的时，迭代器是个所谓的 smart pointers。所有
容器类别都提供一些成员函数，使得我们得以获得迭代器并以之遍历所有元素。

- begin(): 返回指向容器开头的迭代器。
- end(): 返回指向容器结束的迭代器。

```cpp
#include <list>
#include <iostream>

int main() {
  std::list<char> coll;

  for (char c = 'a'; c <= 'z'; ++c) {
    coll.push_back(c);
  }

  for (std::list<char>::const_iterator it = coll.begin(); it != coll.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;
}
```

这里有两个问题需要强调：
1. const 问题
```cpp
const std::list<char>::iterator cit = coll.begin();
++cit;     // 错误，因为 cit 是 const 的，不可更改
*cit = 12; // 正确，虽然 cit 是 const 的，但是并不意味着它里面存储的指针也是 const 的
std::list<char>::const_iterator itc = coll.begin();
++itc;     // 正确，因为 itc 不是 const 的，可以被更改
*itc = 12; // 错误，因为 itc 里面存储的指针是一个 const 指针，无法对数据进行更改。
```

2. ++it 还是 it++
本质上，it++ 的实现，会调用++it。++it 将 it 自增后，返回 it 自身。而 it++ 的操作则是先制作
一份 it 的副本，然后再自增 it，最后返回 it副本。因此后者效率要比前者更低，所以推荐使用前缀递
增而非后缀。

### 关联式容器实例

### 迭代器分类
STL 将迭代器分为五类，按照功能从弱到强分别是：
1. 输入迭代器：只读，且只能读一次，只能向前 ++it
2. 输出迭代器：只写，且只能写一次，只能向前
3. 前向迭代器：可读可写，可以多次读写，只能向前
4. 双向迭代器：可读可写，可以多次读写，可以向前可以向后 ++it, --it
5. 随机访问迭代器：可读可写，可以多次读写，可以随机取值 ++it, --it, it + n, it - n, it[n]

STL 预先定义好的所有容器，其迭代器均属于 双向迭代器 和 随机访问迭代器。(不包括 adapter，比如
stack, queue 等等)
- 双向迭代器: list、set、multiset、map、multimap
- 随机访问迭代器: vector、deque、string

## 4. 算法

## 5. 迭代器之适配器

## 6. 更易型算法

## 7. 使用者自定义之泛型函数

## 8. 以函数作为算法的参数

## 9. 仿函数

## 10. 容器内的元素

## 11. STL 内部的错误处理和异常处理

## 12. 扩展 STL