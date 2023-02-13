# 构造/析构/赋值运算

## 条款05: 了解 c++ 默默编写并调用哪些函数
---
- 编译器可以暗自为 class 创建 default 构造函数、copy 构造函数、copy assignment 操作符 以及 析构函数。
---

即使你写了一个空类，编译器仍然会为它声明一些方法：

```cpp
class Empty {};
// 等价于
class Empty {
public:
    Empty() {}
    Empty(const Empty& rhs) {}
    ~Empty() {}

    Empty& operator=(const Empty& rhs) {}
};
```

编译器创建的这些函数中 default constructor/destructor 主要用来给编译器用来防止 “幕后” 代码，例如调用 base 
classes 和 non-static 成员变量的构造函数和析构函数。

至于 copy 构造函数 和 copy assignment 操作符，编译器创建的版本只是简单的将每一个 non-static 成员变量拷贝到
目标对象。

当编译器无法产生合适的 copy assignment 时，它会放弃产生。

```cpp
template <class T>
class NamedObject {
public:
    NameObject(std::string& name, const T& value);
private:
    std::string& nameValue;
    const T objecteValue;
};
```

显然上面这个类无法生成 copy assignment，因为 nameValue 是一个 reference，而 reference 没有办法被重新赋值。

```cpp
class Base {
private:
    Base& operator=(const Empty& rhs){}
};

class Derive : public Base { }
```

上面的 Derive 类也不会产生 copy assignment，因为 base class 将 operator= 设置为了 private，因此并不可以
使用赋值的方式处理 Base 对象，也就无法正确处理 Derive。

## 条款06: 若不想使用编译器自动生成的函数，就该明确拒绝
---
- 为驳回编译器自动提供的机能，可以将相应的成员函数声明为 private 并且不予实现。使用像 Uncopyable 这样的 base class 也是一种方法。
---
有时候，我们不希望某些编译器自动生成的函数，那么我们就需要用某种方法拒绝这些函数的生成与调用。

对于 default constructor，只需要自行编写其他 constructor 就会避免其被调用。

对于 copy constructor/assignment 则不一样，可以使用 private 来限制其调用。

```cpp
class HomeForSale {
private:
    HomeForSale(const HomeForSale&);
    HomeForSale& operator=(const HomeForSale&);
};
```

用这种方式实现 class 可以当客户企图拷贝 HomeForSale 对象，编译器会阻挠他。如果你不慎在 member 函数或者 friend 
函数中这么做，则连接器会发出报错。

使用继承一个 private copy base class 的方式可以将报错提前至编译期。

```cpp
class Uncopyable {
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:
    Uncopyable(const Uncopyable&);
    Uncopyable& operator=(const Uncopyable&);
};

class HomeForSale : public Uncopyable {};
```

上面这种实现非常微妙，但是功能强大。除了这种方法，你可以使用(继承) Boost 提供的版本，类名为 noncopyable。

## 条款07: 为多态基类声明 virtual 析构函数
---
- polymorphic base classes 应该声明一个 virtual 析构函数。任何拥有 virtual 函数的 class 应该拥有一个 virtual destructor。
- classes 的设计目的如果不是作为 base classes 使用，或不是为了具备多态性，就不该声明 virtual 析构函数。
---

在继承体系中，可能出现的一个情况是使用 factory 函数返回一个指向 新生成的 derived class 对象的 base class 指
针。当需要进行清理，调用 delete 时则需要调用 dervied class 对象的 destructor 函数，否则很容易出现 “局部销毁” 
现象。使用 virtual destructor 能够很好的避免这个问题。

```cpp
class TimeKeeper {
public:
    TimeKeeper();
    virtual ~TimeKeeper();
};

class AtomicClock : public TimeKeeper {};
class WaterClock : public TimeKeeper {};
class WristWatch : public TimeKeeper {};

TimeKeeper* ptk = getTimeKeeper();
delete ptk;
```

通常 base class 都会使用一个 virtual destructor，同时也可能使用其他 virtual function。而如果一个 class 不
企图当作 base class，而令其析构函数为 virtual 则是一个馊主意。这主要与 virtual 函数的实现细节有关系，它通过
vptr 和 vitual table 实现。这会使得对象的存储大小增加，同时也会降低可移植性。

最后得出的心得是：只有当 class 内含有至少一个 virtual 函数，才为它声明 virtual 析构函数。

还需要注意的是一些STL中不提供 virtual  的 class，当发生继承关系时要慎重考虑 delete。

```cpp
class SpecialString : public std::string {};
SpecialString* pss = new SpecialString("Impending Doom");
std::string* ps;
ps = pss;
delete ps; // 错误，因为 string 并不提供 virtual destructor，因此会发生局部销毁而导致资源泄露
```

有时候希望拥有一个抽象 class，这就需要 pure virtual 函数。而有时没有办法找到一个好的 virtual 函数，可以使用 
pure virtual destructor。但是一定要提供一份定义，因为后续继承会调用。

```cpp
class AWOV {
public:
    virtual ~AWOV() = 0;
};

AWOV::~AWOV() {}
```

最后仍要强调 “virtual 析构函数” 只适用于带多态性质的 base classes 身上，如果没有多态性质，该操作只会徒增成本。

## 条款08: 别让异常逃离析构函数

```cpp
class Widget {
public:
    ~Widget();
};

void doSomething {
    std::vector<Widget> v;
}
```

上述代码中，在 doSomething 的结尾 vector v 会被销毁，它有责任销毁其中的所有 Widget。而销毁过程中，如果产生
两个异常，则程序会自动停止。

因此最佳方法是在 destructor 中就处理异常，并且避免在 catch 子句中产生新的异常，或者可以完全不做多余处理。

```cpp
class DBConnection {
public:
    static DBConnection create();
    void close();
};

class DBConn {
public:
    ~DBConn() {
        try {
            db.close();
        } catch (...) {}
    }
private:
    DBConnection db;
};
```

## 条款09: 绝不再构造和析构过程中调用 virtual 函数

```cpp
class Transaction {
public:
    Transaction();
    virtual void logTransaction() const = 0;
};

Transaction::Transaction() {
    logTransaction();
}

class BuyTransaction : public Transaction {
public:
    virtual void logTransaction() const;
}

class SellTransaction : public Transaction {
public:
    virtual void logTransaction() const;
}

BuyTransaction b;
```

在创建 b 时，会先调用 base class 的 constructor，这也就会先调用 base class 的 virtual logTransaction。