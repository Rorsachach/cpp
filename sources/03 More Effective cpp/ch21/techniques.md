# 技术 (Techniques, Idioms, Patterns)

## 条款25：将 constructor 和 non-member function 虚化

### 将 constructor 虚化
所谓 virtual constructor 是某种函数，根据其所获得的输入，产生不同类型的对象。

```cpp
class NLComponent {
public:
};

class TextBlock : public NLComponent {
public:
};

class Graphic : public NLComponent {
public:
};

class NewsLetter {
public:
  NewsLetter(istream &str);
private:
  list<NLComponet *> components;

  static NLComponent * readComponent(istream &str);
}

NewsLetter::NewsLetter(istream &str) {
  while (str) {
    components.push_back(readComponent(str));
  }
}
```

上面这段代码中的 readComponent 就是一个 virtual constructor。它通过获取的输入来向外生成 
TextBlock 或者 Graphic。

一种特别的 virtual constructor 被成为 virtual copy constructor，它将调用这复制一个新的
副本并返回其指针。

```cpp
class NLComponent {
public:
  virtual NLComponet * clone() const = 0;
};

class TextBlock : public NLComponent {
public:
  virtual TextBlock * clone() const {
    return new TextBlock(*this);
  }
};

class Graphic : public NLComponent {
public:
  virtual Graphic * clone() const {
    return new Graphic(*this);
  }
}
```

可以看到，virtual copy constructor 实际上也是调用真正的 copy constructor 来实现。前者只
是利用 “虚函数之返回类型” 规则中的一个宽松点来实现。

当 NLComponent 拥有一个 virtual copy constructor 后，我们可以轻松的为 NewsLetter 实现
一个正常的 copy constructor。

```cpp
class NewsLetter {
public:
  NewsLetter(const NewsLetter &rhs);
private:
  list<NLComponent *> components;
};

NewsLetter::NewsLetter(const NewsLetter &rhs) {
  list<MLComponet *>::const_iterator it = rhs.components.begin();
  for (; it != rhs.components.end(); ++it) {
    components.push_back((*it)->clone());
  }
}
```

### 将 Non-Member Functions 的行为虚化
和 constructor 一样，non-member functions 无法真正意义上的虚化。但是有些场景下，我们仍然
希望能能够面向不同的对象，同一个函数实现不一样的功能。

这里以打印为例，operator<< 无法成为一个 member function 来使用，因此需要寻找其他方法。正确
的行为是，单独声明一个虚函数来执行打印操作，同时在 non-member functions 中调用该虚函数。

```cpp
class NLComponent {
public:
  virtual ostream &print(ostream &os) const = 0;
};

class TextBlock : public NLComponent {
public:
  virtual ostream &print(ostream &os) const;
};

class Graphic : public NLComponent {
public:
  virtual ostream &print(ostream &os) const;
}

inline ostream &operator<<(ostream &os, const NLComponent &c) {
  return c.print(os);
}
```

## 条款26：限制某个 class 所能产生的对象数量
有时，我们需要对使用的对象个数进行限制，本条款用来讨论这些问题。

### 1. 允许 0 或 1 个对象
从 0 开始讨论是为了弄清楚一个最关键的问题——如何阻止对象被产生出来。

首先，阻止一个对象的创建最简单的方法就是 将 constructor 设置为 private。这样一来就不再有可能
调用 constructor 来创建对象。

```cpp
class CantBeInstantiated {
private:
  CantBeInstantiated();
  CantBeInstantiated(const CantBeInstantiated &rhs);
}
```

有时，我们希望只存在一个对象，而并非不创建对象。这样就需要选择性的解除上面的约束限制。

```cpp
class PrintJob;

class Printer {
public:
  void submitJob(const PrintJob &job);
  void reset();
  void performSelfTest();

friend Printer &thePrinter();
private:
  Printer();
  Printer(const Printer &rhs);
}

Printer &thePrinter() {
  static Printer p;
  return p;
}
```

以上代码能够生成唯一的一个 Printer 对象的原因具体有三点：
1. 构造函数设置为 private，阻止其他人调用生成 Printer 对象。
2. thePrinter 被声明为 class 的一个 friend，使得其可以调用 private constructor。
3. 使用 static 进行限制，限制全局只有一个对象。

上面的 thePrinter 函数是作为一个全局函数出现的。如果你认为不妥，可以使用 静态成员方法 或者 
namespace 来解决这个问题。

在上面的代码中，还有两个细节值得讨论：

**1. Printer 对象是一个 函数中的 static 对象 而非 class 中的 static 对象。**
这两者的区别在于，class static object 当类型所在的文件被加载时就会被创建。而 function static 
objcet 则是在第一次调用时才被创建。

**2. 如此短小的函数为什么不使用 inline**
这需要理解一下 inline 的含义。除了概念上的编译时进行替代以外，对于 non-member functions，它
还意味着 这个函数有内部连接。

函数如果带有内部链接，可能会在程序中被复制。这也就意味着 static 对象可能会拥有多份该 static 对
象的副本。因此谨记：

> [!warning]
> 千万不要产生内含 local static 对象的 inline non-member functions。

除了这种方案外，是否还有更加简单易懂的方案。有，当存在对象超过限额时抛出异常。

```cpp
class Printer {
public:
  class TooManyObjects {};

  Printer();
  ~Printer();
private:
  static size_t numObjects;
}

size_t Printer::numObjects = 0; // class statics 除了在 class 内声明，还需要在 class 外定义

Printer::Printer() {
  if (numObjects >= 1) {
    throw TooManyObjects();
  }

  ++numObjects;
}

Printer::~Printer() {
  --numObjects;
}
```

这种方式更容易一般化，只需更改限额数量。

### 2. 不同的对象构造状态
但是上面这种方法仍然有问题。当存在继承关系，或者被嵌入在其他对象中时可能会产生与预期不同的结果。

```cpp
class ColorPrinter : public Printer {}

Printer p;
ColorPrinter cp;
```

因为，当子类调用 constructor 时，会调用父类的无参构造函数。因此上面这种本意是 printer 和 
color printer 各一个的场景却会抛出 exception。因为申请了两个 printer。

同样的事情也会发生在内含 Printer 对象的其他对象中。

```cpp
Class CPFMachine {
private:
  Printer p;
  FaxMachine f;
  CopyMachine c;
};

CPFMachine m1;
CPFMachine m2;
```

上述代码中，当生成 m2 时会抛出 exception，原因同上。

使用 private constructors 的 class 来实现，可以导致不可派生。如此一来可以实现包括两个特点的
对对象：1. 有限个对象。2. 不可被继承。

```cpp
class FSA {
public:
  static FSA * makeFSA();
  static FSA * makeFSA(const FSA &rhs);

private:
  FSA();
  FSA(const FSA &rhs);
}

FSA * FSA::makeFSA() {return new FSA();}
FSA * FSA::makeFSA(const FSA &rhs) {return new FSA(rhs);}
```

上述的代码完全满足两个特性，首先可以被新建多个对象，同时使用前面的技术可以限制产生对象的数量。其
次将 constructor 写为 private 函数，防止继承。

但是需要注意的是，其中使用 new 进行资源分配。所以必须记得调用 delete 来阻止资源的泄漏。或者，
可以选择使用 auto_ptr 来封装对象，在离开 scoop 时自动删除这些对象。

```cpp
auto_ptr<FSA> pfsa1(FSA::makeFSA());
auto_ptr<FSA> pfsa2(FSA::makeFSA(*pfsa1));
```

### 3. 允许对象生生灭灭
在 1 和 2 中已经了解到了如何设计 “只允许单一对象” 的 class，“追踪某特定 class 的对象个数”，
以及计数会导致的 constructor 调用异常，使用 private constructor 来防止这种事情的发生。

但是，当限制了个数为 1 的对象时，又很难实现在不同时间内维持 1 个对象的需求。

```text
创建 Pointer p1;
使用 p1;
销毁 p1;

创建 Pointer p2;
使用 p2;
销毁 p2;
```

这种场景也是可预期的有限个数，但是与 [### 1.] 中所提到情形却不相同。实现这种效果的方法是：将
“对象计数” 和 “伪构造函数” 相结合。

```cpp
class Printer {
public:
  class TooManyObjects{};

  static Printer * makePrinter();
  ~Printer();
  void submitJob(const PrintJob &job);
  void reset();
  void performSelfTest();

private:
  static size_t numObjects;
  Printer();
  Printer(const Printer &rhs);
}

size_t Printer::numObjects = 0; // class statics 除了在 class 内声明，还需要在 class 外定义

Printer::Printer() {
  if (numObjects >= 1) {
    throw TooManyObjects();
  }

  ++numObjects;
}

Printer * Printer::makePrinter() {
  try {
    return new Printer();
  }
  catch (TooManyObjects &e) {
    return nullptr;
  }
}

Printer::~Printer() {
  --numObjects;
}
```

上面代码所提供的 makePrinter 处理了 exception，因为有时我们不希望这个或称带来额外的困扰。但
是这也就意味着返回 nullptr，使用者需要对 makePrinter 进行验证。

### 4. 一个用来计算对象个数的 base class
对上述代码进行进一步的优化，其中的计数部分可以被单独提取出来写为一个 base class，作为对象计数
用。然后将诸如 Printer 的类来继承它。

```cpp
template <class BeingCounted>
class Counted {
public:
  class TooManyObjects {};
  static int objectCount();

protected:
  Counted();
  Counted(const Counted &rhs);
  ~Counted(){ --numObjects; }

private:
  static int numObjectes;
  static const size_t maxObjects;
  void init();
};

template <class BeingCounted>
Counted<BeingCounted>::Counted() {
  init();
}

template <class BeingCounted>
Counted<BeingCounted>::Counted(const Counted<BeingCounted>&) {
  init();
}

template <class BeingCounted>
void Counted<BeingCounted>::init() {
  if (numObjects >= maxObjects) throw TooManyObjects();
  ++numObjects;
}

class Printer : private Counted<Printer> {
public:
  static Printer * makePrinter();
  static Printer * makePrinter(const Printer &rhs);
  ~Printer();
  void submitJob(const PrintJob &job);
  void reset();
  void performSelfTest();

  using Counted<Printer>::objectCount;
  using Counted<Printer>::TooManyObjects;

private:
  Printer();
  Printer(const Printer &rhs);
}
```

Printer 使用 Counted template 追踪目前存在的 Printer 对象。这种实现使用 private 继承。
如果使用 public 继承，那么就需要为 Counted classes 提供一个 virtual destructor，否则当
有人通过 Counted<Printer> * 指针来删除 Printer 对象时会产生意外的效果。

因为使用了 private 继承，因此 Counted classes 中的 public/protected 内容都变为了 private 
内容。所以使用 using declaration 来恢复 public 访问层级。

使用上面的方案，可以轻松的将计数操作忽略，而方便 Printer 类似类型的设计。

其中最后需要被讨论的一个点是：关于 maxObjects 的值的定义。这个定义应该由 Printer 的作者进行
定义，他需要在实现文件中添加 `const size_t Counted<Printer>::maxObjects = 10;` 这样的
定义行。

## 条款27：要求 (或禁止) 对象产生于 heap 之中
部分时候，我们希望某些对象绝对不要从 heap 中分配，而另一部分一定要从 heap 中分配得到。

### 1. 要求对象产生于 heap 之中(heap-based object)
non-heap objects 会自动调用构造以及析构函数。那么为了强制使用 new 在 heap 之上构建对象，就
可以从 constructor 和 destructor 下手。

将 destructor 设置为 private，并设计 伪destructor

```cpp
class UPNumber {
public:
  UPNumber();
  UPNumber(int initValue);
  UPNumber(double initValue);
  UPNumber(const UPNumber &rhs);

  void destroy() const { delete this; }

private:
  ~UPNumber();
};

UPNumber n; // 错误，因为离开 scoop 时需要调用 destructor，但是却是 private 的
UPNumber *p = new UPNumber(); // 正确使用方法
p->destroy();
```

或者，另一个办法时将所有的 constructors 都声明为 private。这种行为的麻烦之处在于，这需要限制
多个 constructor 包括 copy/default constructor 等等。

通过 private 来限制 constructor/destructor 很有效果，但也有副作用。比如 26 中所描述的继承
和内含行为会变得非法。解决方法也非常简单：
- 继承：使用 protected 而非 private 来限制 destructor 便可以解决继承问题。
- 内含：通过指针来引用 UPNumber 对象，而非直接使用对象可以解决内含问题。

### 2. 判断某个对象是否在 heap 内

```cpp
class UPNumber {
  ...
protected:
  ~UPNumber();
};
class NonNegativeUPNumber : public UPNumber {};

NonNegativeUPNumber n;
```

按照上面的这种继承策略，仍然可以创建一个 non-heap 的对象 n。那么有没有可能将 UPNumber 的所有
派生类也都限制在 heap 之上？

事实上没有什么简单办法来实现。我们需要判断某个对象是否在 heap 上，如果仅仅通过在 operator new 
中设立位值的方式来判断是不行的。

大多数系统的地址空间分布是按照下面排布的

```text
-------------
|   Stack   | 高位地址
-------------
|           |
|           |
|           |
-------------
|    Heap   | 低位地址
-------------
```

因此，一个简单的逻辑是，如果当前地址比一个 stack 对象的地址还要高，那么一定不在 heap 中。

```cpp
// 不一定准确
bool onHeap(const void *address) {
  char onTheStack;
  return address < &onTheStack;
}
```

除了这个问题外，实际上 static 变量有时候也和 heap 地址空间相接，而这是无法区分的。

换个角度想，当你在纠结 “对象是否在 heap 内” 时，实际上可能是因为你想知道 它调用 delete 是否
安全。幸运的是，这种删除动作是否安全的判断是比较容易解决的，因为这只需要判断 “此指针是否由 new 
返回”。一个简单的逻辑思路是

```cpp
void *operator new(size_t size) {
  void *p = getMemory(size); // 分配一个 size 大小的内存
  // 将 p 添加到某个固定的地址集合中
  return p; // 返回 p
}

void operator delete(void *ptr) {
  releaseMemory(ptr); // 将内存归还给 free store
  // 将 ptr 从 collection 中移除
}

bool isSafeToDelete(const void *address) {
  // 返回是否 address 在某个集合中
}
```

这样设计存在三个问题：
1. 我们极端不愿意在全局空间内定义任何东西，尤其是具有缺省意义的函数
2. 没有必要为所有的 heap 应用提供沉重的记录工作
3. 似乎不可能实现出一个总是能够有效作用的 isSafeToDelete 函数。比如涉及多重继承或虚拟继承的基类时

事实上，我们需要的是一个既能提供这些机能，又不附带全局命名空间的污染问题、额外的义务性负荷，以及
正确性疑虑的东西。c++ 的 abstract mixin base class(抽象混合式基类) 能够完全满足我们的需求。

abstract base class 是一个至少包含一个纯虚函数的不可实例化的 base class。mixin class 则
提供一组定义完好的能力，能够与其 derived class 所可能提供的其他任何能力兼容。我们可以用一个所
谓的 abstract mixin base class 来为 derived classes 提供 “判断某指针是否以 operator 
new 分配出来” 的能力。

```cpp
class HeapTracked {
public:
  class MissingAddress();
  virtual ~HeapTracked() = 0;
  static void *operator new(size_t size);
  static void operator delete(void *ptr);
  bool isOnHeap() const;

private:
  typedef const void *RawAddress;
  static list<RawAddress> addresses;
};

list<RawAddress> HeapTracked::addresses;
HeapTracked::~HeapTracked() {}

void *HeapTracked::operator new(size_t size) {
  void *memPtr = ::operator new(size); // 取得内存，
  address.push_front(memPtr); // 存储地址
  return memPtr;
}

void HeapTracked::operator delete(void *ptr) {
  list<RawAddress>::iterator it = find(addresses.begin(), addresses.end(), ptr);
  if (it != addresses.end()) {
    addresses.erase(it);
    ::operator delete(ptr);
  } else {
    throw MissingAddress();
  }
}

bool HeapTracked::isOnHeap() const {
  const void *rawAddress = dynamic_cast<const void *>(this);
  list<RawAddress>::iterator it = find(addresses.begin(), addresses.end(), rawAddress);

  return it != address.end();
}

class Asset : public HeapTracked {
private:
  UPNumber value;
}

void inventoryAsset(const Asset *op) {
  if (op->isOnHeap()) {

  } else {

  }
}
```

在上面的这段代码中，唯一可能存在困惑的就是:`const void *rawAddress = dynamic_cast<const void *>(this);`
凡是涉及 “多重或虚拟基类” 的对象，都会拥有多个地址。而使用 dynamic_cast 则可以用来消除这个问
题，只需要简单的将指针 “动态转型” 为 void *。

### 3. 禁止对象产生于 heap 中
这主要包括三种一般情况：
1. 对象被直接实例化
2. 对象被实例化为 derived class objects 中的 base class 成分
3. 对象被内嵌于其他对象之中

想要禁止产生于 heap，那么必须要从 new operator 和 operator new 下手。前者是语言内建的，没
有可更改的空间，但是后者却是可以自由修改的。操作与前面类似，将 operator new/delete 声明为 
private 即可。

```cpp
class UPNumber {
private:
  static void *operator new(size_t size);
  static void operator delete(void *ptr);
  static void *operator new[](size_t size); // 如果想禁止形成数组
  static void operator delete[](void *ptr);
}
```

这种操作也会应影响 derived class objects 中的 base class 成分。如果 derived class 没有
声明自己的 public operator new，那么它会继承 base 的 private 版本。如此一来无法 derived
也无法使用 new 来创建。

TODO：

## 条款28：Smart Pointer (智能指针) (*)
当你使用 smart pointer 来取代 c++ 的内建指针时，你就会获得下面各种指针行为的控制权：
- 构造和析构：你可以决定 smart pointer 什么时候被销毁以及销毁时如何做。
- 复制和赋值：当一个 smart pointer 被复制或涉及赋值动作时，你可以控制发生什么事。
- 解引用：当 client 解引 smart pointer 所指之物时，有权决定发生什么事情。

smart pointers 由 templates 产生，它就像内建指针一样，具有强类型性(strongly typed)。一
个 smart pointers 类似下面这种结构

```cpp
tempalte <class T>
class SmartPtr {
public:
  // 构造与析构过程
  SmartPtr(T* realPtr = 0);
  ~SmartPtr();
  // 复制与赋值过程
  SmartPtr(const SmartPtr& rhs);
  SmartPtr& operator=(const SmartPtr& rhs);
  // 析构过程
  T* operator->() const;
  T& operator*() const;
private:
  T* pointee; // 指针指向的内容
};
```

### smart pointer 使用示例
考虑一个分布式系统，其中某些对象位于 local，某些位于 remote。本地对象的访问通常比远程对象的
访问简单且快速。对于应用程序而言，如果本地对象和远程对象的处理方式有所不同，是件挺麻烦的事情。
让所有对象都好像位于本地，应该是比较方便的做法。

```cpp
template <class T>
class DBP突然{
public:
  DBPtr(T* realPtr = 0);
  DBPtr(DataBaseID id);
};

class Tuple {
public:
  void displayEditDialog();
  bool isValid() const;
};

template <class T>
class LogEntry {
public:
  LogEntry(const T& obj);
  ~LogEntry();
};

void editTuple(DBPtr<Tuple>& pt) {
  LogEntry<Tuple> entry(*pt);
  do {
    pt->displayEditDialog();
  } while (pt->isValid() == false);
}
```

在上面这段代码中，editTuple 的编写者并不在乎 pt 指向的是本地的还是远程的数据，因为 DBPtr 会
完成这些操作。另外一个 tips 是，LogEntry 对象并不像传统设计那样，以 “开始运转记录” 和 “结束
运转记录” 等函数调用动作。而是使用 constructor 和 destructor 来进行操作。

### Smart Pointers 的构造、赋值、析构

#### 构造函数
smart pointers 的 constructor 非常容易理解：确定一个目标物，然后让 smart pointer 内部的
dumb pointer 来指向目标物。

smart pointers 的 copy constructor、assignment operators 和 destructor 的实现却因为
“拥有权”的观念而变得稍微复杂。例如删除操作，只有 dumb pointer 指向一个来自 heap 的对象时 delete 
才会生效。

#### 复制与赋值
在进行复制时，多个 smart pointer 指向同一个对象，然后每个 smart pointer 都试图调用 delete 
时，就可能出现 nullptr 的情况。

想要解决这个问题，方法一，创先一个新的副本来防止多个指针指向同一个对象，此时就要设计 virtual 
constructors。方法二，禁止复制和赋值。方法三，在 auto_ptr 中采用了一种更具弹性的解决方法，
当 auto_ptr 被复制或被赋值，其 “对象拥有权” 会转移。

```cpp
template<class T>
class auto_ptr {
public:
  auto_ptr(auto_ptr<T>& rhs);
  auto_ptr<T>& operator=(auto_ptr<T>& rhs);
private:
  T* pointee;
};

template <class T>
auto_ptr<T>::auto_ptr(auto_ptr<T>& rhs) {
  // 将rhs.pointee对象拥有权转移给 *this
  pointee = rhs.pointee;
  rhs.pointee = 0;
}

template <class T>
auto_ptr<T>& auto_ptr<T>::operator=(auto_ptr<T>& rhs) {
  if (this == &rhs) return *this;
  
  delete pointee;
  pointee = rhs.pointee;
  rhs.pointee = 0;

  return *this;
}
```

由于 auto_ptr 的 copy constructor 被调用时，对象拥有权被转移了，所以以 by value 的方式
传递 auto_ptr 是一个糟糕的决定。

```cpp
void printTreeNode(ostream& os, auto_ptr<TreeNode> p) {
  os << p;
}

int main() {
  auto_ptr<TreeNode> ptn(new TreeNode());
  printTreeNode(std::cout, ptn);
}
```

上述代码中，p 被以 copy constructor 的方式进行初始化，此时 ptn 中指向的对象被转移到了 p 这
个局部变量中。随着 printTreeNode 的结束，p 被释放也就导致 new TreeNode 创建的对象被销毁。

#### 析构

```cpp
template<class T>
SmartPtr<T>::~SmartPtr() {
  if (*this owns *pointee) {
    delete pointee;
  }
}
```

上面是一个 destructor 的通用写法。有时候没必要做测试，例如当 auto_ptr 所指之物的拥有权总是
在当前 auto_ptr 之上。有时候又需要更复杂的测试，例如使用引用计数的 shared_ptr，它必须决定
是否进行删除之前对计数器进行调整。

### 实现 Dereferencing Operators (解引操作符)

```cpp
template <class T>
T& SmartPtr<T>::operator* () const {
  // 执行有关 smart pointer 相关的处理过程
  return *pointee;
}
```

上面是一个 operator* 的基本实现方式。

返回值一定要是一个 reference，有几点好处。1. 防止 slicing 的发生，因为返回值可能是 T 的继
承类型，如果没有使用 reference，而是直接返回对象，会发生初始化操作，这时就会发生 slicing。
得到的新对象不会调用动态类型对应的函数。（这一点在前面提到过）2. reference 的效率较高。

operator-> 和 operator* 差不多，当一个调用 `pt->displayEditDialog()` 发生时，等价于 
`(pt.operator->())->displayEditDialog()`。此时 operator-> 只会返回两种东西：1. dumb 
pointer 2. smart pointer。大多数情况下你会希望返回一个 dumb pointer。

```cpp
template <class T>
T* SmartPtr<T>::operator-> () const { // 注意这里是直接返回一个指针
  // 执行有关 smart pointer 相关的处理过程
  return pointee;
}
```

### 测试 Smart Pointers 是否为 Null
对 smart pointers 如何判空是一个需要考虑的问题。显然，当前的设计没有办法完成该工作。一种方
法是设计一个 isNull 函数来实现判空。

```cpp
template <class T>
bool SmartPtr<T>::isNull() const {
  return pointee == 0;
}
```

但是这让 smart pointer 与 dumb pointer 的判空方式截然不同。为了能够让两者方式一致，一种方
法是通过隐式类型转换，将其转换为 void* 来实现。

```cpp
template <class T>
SmartPtr<T>::operator void*() {
  return static_cast<void*>(pointee);
}
SmartPtr<int> pInt;
if (pInt == 0)
if (pInt)
if (!pInt)
```

这种实现方法和 iostream classes 很类似。

```cpp
ifstream inputFile("datafile.dat");
if (inputFile)
```

但是这种方法的缺点是，有些看上去就应该调用失败的做法却成功编译了。

```cpp
SmartPtr<Apple> pa;
SmartPtr<Orange> po;
if (pa == po)
```

上述代码中，我们并没有重写 operator== 操作符，但是却通过了编译。因为二者都自动转换为了 void* 
然后进行比较。这也符合条款5所说的那样，对定制的类型转换操作符警惕。

为了降低这种意外发生的概率，一个很好的方法是设计 operator! 而不是使用 void* 类型转换。

```cpp
template <class T>
bool SmartPtr<T>::operator!() {
  return pointee == 0;
}
SmartPtr<int> pInt;
if (!pInt)
```

这种情况可以将风险降到最低，但是只能使用 !ptr 来进行判空，而不能使用 ptr 或者 ptr == 0。除
此之外还有一个唯一的风险，但是很不常见，因为几乎没有人这样写代码。

```cpp
SmartPtr<Apple> pa;
SmartPtr<Orange> po;
if (!pa == !po)
```

### 将 Smart Pointers 转换为 Dumb Pointers
对于某些老式的接口，他们或许没有提供 smart pointers，所以为了能够适用其中，你希望将 smart 
pointer 转换为 dumb pointer。使用 `&*pt` 是可以的，但是不简洁。一种方法就是设计隐式类型
转换。

```cpp
template <class T>
SmartPtr<T>::operator T*() {
  return static_cast<T*>(pointee);
}
```

这种转换方法，不仅解决了 smart pointer 转向 dubm pointer 的问题，同时也解决了 smart pointer 
是否为 null 的问题。但是正如前面所提到的那样，隐式转换带来了其他负面影响。

```cpp
void processTuple(DBPtr<Tuple>& pt) {
  Tuple* rawTuplePtr = pt; // 隐式转换
  ...
}
```

使用者很容易获取到 dumb pointer 并进行相关操作，这种行为是十分危险的。

另外的一个问题是，c++ 编译器不允许连续多次的转换。

```cpp
class TupleAccessors {
public:
  TupleAccessors(const Tuple* pt);
};

TupleAccessors merge(const TupleAccessors& ta1, const TupleAccessors& ta2);
DBPtr<Tuple> pt1, pt2;
merge(pt1, pt2);
```

上面这段代码是无法通过编译的，因为这要经过两次转换，首先是 pt1 pt2 从 smart pointer 转向 
dumb pointer，然后是 dumb pointer 转换为 TupleAccessors 类型。这种连续的转换行为，c++ 
是不允许的。

还有一种情况发生在 delete 过程中。

```cpp
DBPtr<Tuple> ptr = new Tuple;
delete ptr;
```

上述代码本来不应该运行成功，因为 ptr 并非是 new 出来的，因此不能调用 delete。但是却运行成功
了，因为 ptr 被隐式转换为了 Tuple* dumb pointer，此时指向的区域正好就是 new 出来的，因此
成功了。然后当退出 scoop 时，ptr 被自动释放，调用 destructor，此时又将调用一次 delete，那
么产生了异常。

所以总的来讲，千万不要提供对 dumb pointers 的隐式转换操作符，除非迫不得已。

### Smart Pointers 和 “与继承有关的” 类型转换

```cpp
class MusicProduct {
public:
  MusicProduct (const string& title);
  virtual void play() const = 0;
  virtual void displayTitle() const = 0;
};
class Cassette : public MusicProduct {
  Cassette(const string& title);
  virtual void play() const;
  virtual void displayTitle() const;
};
class CD : public MusicProduct {
  CD(const string& title);
  virtual void play() const;
  virtual void displayTitle() const;
};

void displayAndPlay(const SmartPtr<MusicProduct> pmp, int numTimes) {
  for (int i = 0; i < numTimes; ++i) {
    pmp->displayTitle();
    pmp->play();
  }
}

SmartPtr<Cassette> funMusic(new Cassette("jazz"));
SmartPtr<CD> nightmareMusic(new CD("disco"));
displayAndPlay(funcMusic, 10);
displayAndPlay(nightmareMusic, 0);
```
 
上述代码无法通过编译，因为 SmartPtr<MusicProduct> SmartPtr<Cassette> 和 SmartPtr<CD> 
是完全不相关的，而不存在继承关系。

或许你想通过全特化来通过编译。

```cpp
class SmartPtr<Cassette> {
public:
  operator SmartPtr<MusicProduct>() {
    return SmartPtr<MusicProduct>(pointee);
  }
};
```

这显然是不现实的，因为你需要对每一个类型都写出如上操作，这违背了 template 的本意。

c++ 中一种新的特性可以满足这种需求，它可以将 non-virtual member function 声明为 templates。

```cpp
template<class T>
class SmartPtr {
public:
  SmartPtr(T* realPtr = 0);
  ...

  template <class newType>
  operator SmartPtr<newType>() {
    return SmartPtr<newType> (pointee);
  }
};
```

上述代码的巧妙之处在于，`return SmartPtr<newType> (pointee);` 返回的是一个通过构造函数
创建的对象。在这个过程中，如果 T* 无法被隐式转换为 newType* 是不会成功运行的。这也就保证了
指针转换的原有规则。

这种转换实际上不止适用于继承关系中的向上转换，各种形式 smart pointer 的转换，都可以通过这种方式进
行实现。

利用 member templates 来转换 smart pointer，有两个缺点：
1. 目前支持的编译器可能还不多，移植性不高。
2. 其中涵盖大量的技术要点：函数调用的自变量匹配规则、隐式类型转换函数、template functions 的暗自实例化、member function template 计数。

总结来看，我们无法将 smart pointer classes 的行为在 “与继承相关的类型转换” 上做到和 dumb 
pointer 一样，最多是能够使用 member templates 来产生转换函数。

### Smart Pointers 与 const
首先先回忆一下 dumb pointers 与 const

```cpp
CD goodCD("Flood");
const CD* p; // p 指向一个 const CD object
CD* const p = &goodCD; // p 是一个 const pointer，不能进行修改，但是指向的对象可以被修改
const CD* const p = &goodCD;
```

smart pointers 也可以有这几种情况。

```cpp
CD goodCD("Flood");
SmartPtr<CD> p;
const SmartPtr<CD> p; // const pointer, non-const object
SmartPtr<const CD> p; // non-const pointer, const object
const SmartPtr<const CD> p; // const pointer, const objcet
```

但是 dumb pointers 可以有 指向 non-const 对象的指针作为指向 const 对象指针的初值。

```cpp
CD* pCD = new CD("Jazz");
const CD* pConstCD = pCD;         // 正确

SmartPtr<CD> pCD(new CD("Jazz"));
SmartPtr<const CD> pConstCD(pCD); // 错误
```

smart pointer 无法实现这种操作，和上一小节讲的一样，这是两种类型。那么如果你的编译器支持 member 
template 那你就可以实现一个类型转换操作符来解决这个问题。

但是如果你没法使用 member template 呢？

```cpp
template <class T>
class SmartPtrToConst {
protected:
  union {
    const T* constPointee;
    T* pointee;
  };
};

template <class T>
class SmartPtr : public SmartPtrToConst<T> {
};

SmartPtr<CD> pCD = new CD("Jazz");
SmartPtrConst<CD> pConstCD = pCD;
```

使用继承的方式来实现，在 SmartPtr 中调用 pointee，而在 SmartPtrToConst 中则调用 constPointee，
就实现了一个 const 方案。

## 条款29：Reference counting (引用计数)


## 条款30：Proxy class (替身类、代理类) (*)

## 条款31：让函数根据一个以上的对象类型来决定如何虚化 (*)

