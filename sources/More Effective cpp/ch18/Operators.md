# 操作符

## 条款5：对定制的 “类型转换函数” 保持警觉
c++允许编译器在不同类型之间执行隐式转换。但是部分隐式转换存在安全性问题，例如 int-->short，
double-->char。然而，自己设计类型时可以尝试提供特定的 “函数” 来作为隐式类型转换使用，并保证安
全性。

> [!tip]
> 尽管如此，最好不要提供任何类型转换函数。

实现定制的类型转换函数有两种方案。

### 1. 单自变量 constructors 
单自变量 constructors 指能够以单一自变量成功调用的 constructors，也可以是拥有多个参数，但除
了第一个参数之外都有默认值。

```cpp
class Name {
public:
  Name(const string &s); // 可以将 string 转换为 Name
};

class Rational {
public:
  Rational(int numerator = 0, int denominator = 1); // 可以将 int 转换为 Rational
};
```

### 2. 隐式类型转换操作符
隐式类型转换操作符，是一个拥有奇怪名称的 member function：在关键词 operator 之后加上一个类
型名称，并且不制定返回值类型。

```cpp
class Rational {
public:
  operator double() const; // 将 Rational 转换为 double
}

Rational r(1, 2);
double d = 0.5 * r; // r 自动 转换为 double
```

### 最好不要提供任何类型转换函数
因为在未预期的情况下，此类函数可能会被调用，而结果可能不正确，不直观且难以调试。

#### a. 隐式类型转换操作符
针对隐式类型转换操作符，可能的情况如下：

```cpp
Rational r(1, 3);
std::cout << r; // 期待打印 "1/3"
```

如果 Rational 中并为提供 operator<< 方法，但却提供了隐式类型转换操作符，这时就会发生意想不到
的结果。编译器会将 Rational 转换为 double 进行打印，这显然并不是期待的结果。

为了解决这个问题，应当使用一个函数来取代隐式类型转换操作符，例如 asDouble。

```cpp
class Rational {
public:
  double asDouble() const;
}

Rational r(1, 3);
std::cout << r; // 期待打印 "1/3"
std::cout << r.asDouble() // 期待打印 0.333333
```

#### b. 单自变量 constructors


## 条款6：区别 increment/decrement 操作符的 前置 和 后置 形式
为了区分 ++/-- 的前置形式和后置形式，重载函数以参数类型来区分彼此。

```cpp
class UPInt { // unlimited precision int
public:
  UPInt(int value) : _value(value) {}
  UPInt &operator++() { // 前置形式
    (*this)._value += 1;
    return *this;
  }
  const UPInt operator++(int) { // 后置形式
    UPInt oldValue = *this;
    ++(*this);
    return oldValue;
  }
  UPInt &operator--() {
    (*this)._value -= 1;
    return *this;
  }
  const UPInt operator--(int) {
    UPInt oldValue = *this;
    --(*this);
    return oldValue;
  }
  friend ostream &operator<<(ostream &os, UPInt);
private:
  int _value;
};
```

后置式操作符没有使用传入的参数，只是为了区别前置式和后置式而已。为了避免发出警告，可以故意省略参
数名称。前置式返回一个 reference，而后置式返回一个 const 对象。这种行为主要是为了和内建类型保
持一致，以避免错误。

```cpp
UPInt upi;
++++upi; // 合理
upi++++; // 不合理
```

除此之外，关于效率问题，显然 后置式 通过调用 前置式 来完成任务，同时还生成一个临时对象进行存储。
因此，前置式效率更高。

> [!tip]
> 选择的原则是：除非真的需要后置式的行为(返回 const 对象)，否则使用前置式。

## 条款7：千万不要重载 && || 和 , 操作符
在进行 “真假值” 判断时，c/c++ 采用“骤死式”的判断方案，即从左到右进行判断，如果左面不成立则不继
续判断。这使得这三个操作符具有顺序性，而对其进行重载会导致破环这种秩序性，从而导致意想不到的结果。

因此应该选择不去重载这些操作符。

## 条款8：了解各种不同意义的 new 和 delete

### 1. new
主要需要理解 new operator 和 operator new 之间的差异。

new operator 是类似于 `string *ps = new string("hello world");` 中的new。这个操作符是
由语言内建的，不能被改变。它一般执行两个操作：1. 分配一个足够防止类型对象的内存空间。2. 调用一个
constructor，为刚才分配的内存中那个对象设定初值。

operator new 则是用来申请内存空间的函数，通常这样使用`void *rawMemory = operator new(sizeof(string))`。
此外，operator new还可以被重载，改变其行为。

需要注意的是，只要使用 new operator，必定会默认重新使用 operator new 来分配一块空间，并调用
constructor。通常情况下无法直接在一块已分配的空间上直接调用 constructor。

#### placement new
如果需要在一些分配好的原始内存中，直接调用 constructor 来构建对象。有一个特殊版本的 operator 
new 被成为 placement new，允许你这么做。

```cpp
#include <new> // 使用 placement new 需要用 <new>

class Widget {
public:
  Widget(int widgetSize) : _size(widgetSize) {}
  int size() {return _size;}
private:
  int _size;
};

int main() {
  void *rawMemory = operator new(sizeof(Widget));
  Widget *pw = static_cast<Widget *>(rawMemory);
  new (pw) Widget(12); // 作为placement new 所需的第二个参数制定内存位置
}
```

### 2. deletion and Deallocation
与 new operator 和 operator new 的关系一样，delete operator 和 operator delete 也是
类似却不同的两个操作。

delete operator 与 new operator 相对应，它也执行两步操作：1. 调用对象的 destructor 方法，
从而释放对象中其他引用的内存空间。2. 执行 operator delete 将申请的空间还给内存。

而 operator delete 与 operator new 相对应，只进行空间的释放。这也就对使用者提出了新的要求，
必须手动调用对象的 destructor 方法才行。

```cpp
int main() {
  void *rawMemory = operator new(sizeof(Widget));
  Widget *pw = static_cast<Widget *>(rawMemory);

  new (pw) Widget(12);

  pw->~Widget();
  operator delete(rawMemory);
}
```

需要注意的是，不能直接使用 delete operator 来释放 pw 指针指向的内容，因为该过程并非使用 new 
operator 来实现的，而是使用 placement new 来完成的，会产生错误。

### 3. 数组
operator new/delete 也可以处理数组内容，对应的使用方法是

```cpp
int main() {
  void *rawMemory = operator new[](sizeof(Widget) * 10);

  operator delete[](rawMemory);
}
```