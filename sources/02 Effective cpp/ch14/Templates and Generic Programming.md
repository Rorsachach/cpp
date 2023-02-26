# 模板与泛型编程

## 条款41: 了解隐式接口和编译期多态
**面向对象编程世界**总是以显式接口和运行期多态解决问题。

```cpp
class Widget {
public:
  Widget();
  virtual ~Widget();
  virtual std::size_t size() const;
  virtual void normalize();
  void swap(Widget& other);
};
void doProcessing(Widget& w) {
  if (w.size() > 10 && w != someNastyWidget) {
    Widget temp(w);
    temp.normalize();
    temp.swap(w);
  }
}
```

上述代码中的 doProcessing 就体现了这两点：
- Widget 是一个显式接口
- Widget 中的 virtual 成员函数，在 doSomething 中依据动态类型而变化是运行期多态的体现。

而在 Templates 和 泛型编程的世界中，这两者重要性降低，反而 隐式接口和编译器多态 重要性升高。

```cpp
template<typename T>
void doProcessing(T& w) {
  if (w.size() > 10 && w != someNastyWidget) {
    T temp(w);
    temp.normalize();
    temp.swap(w);
  }
}
```

例如上述代码：
- 上述的执行过程要求 T 类型必须支持 size, normalize, swap, copy, inequality comparison 等行为，这就是一组隐式接口
- 所有涉及到 w 的任何调用都有可能让 template 具象化，这种行为发生在编译期，会导致编译期多态。

## 条款42: 了解 typename 的双重意义

```cpp
template<class T> class Widget;
template<typename T> class Widget;
```

上述代码中 class 与 typename 对于 c++ 而言，意义完全相同。但是推荐，当按时参数并非一定是个
class 类型时使用 typename。

但是并非任何时候 class 总是与 typename 等价。

### 嵌套从属类型名

```cpp
template <typename C>
void print2nd(const C& container) {
  if (container.size() >= 2) {
    C::const_iterator iter(container.begin());
    ++iter;
    int val = *iter;
    std::cout << value;
  }
}
```

上面这段代码接收一个 STL container，并且输出第二号元素。这里面有几个概念：
- 从属名称: template 中出现的名称如果相依于某个 template 参数，就称为从属名称，例如 C 就依赖于 C 类型。
- 嵌套从属名称: 如果丛书名称在 class 内呈现嵌套状，就称为嵌套从属名称，例如 C::const_iterator。
- 非从属名称: 不依赖任何 template 参数的名称，例如 int。

嵌套从属名称可能会导致解析困难的问题，例如

```cpp
template <typename C>
void print2nd(const C& container) {
  C::const_iterator* x;
}
```

上面这段代码如果 C::const_iterator 是一个类，那就没有什么问题。而如果不幸的是传入的类型 C 
恰好声明了一个 static 变量，变量名称叫做 const_iterator。那这就变成了一个相乘行为。这显然
和预期不符。同样的，上面的 `C::const_iterator iter(container.begin())` 也会是一个非法
语句。

想要矫正这个行为可以通过 typename 来限制它只能是一个类型。

```cpp
template <typename C>
void print2nd(const C& container) {
  if (container.size() >= 2) {
    typename C::const_iterator iter(container.begin());
  }
}
```

一般性规则是：任何时候当你想要在 template 中指涉一个嵌套从属类型名称，它就必须在紧邻它的前一
个位置放上关键字 typename。

> [!warning]
> 这一一般性规则的例外是，typename 不剋出现在 base classes list 内的嵌套从属类型之前，也
> 不可以出现在 member initialization list 中作为 base class 修饰符。

```cpp
template <typename T>
class Derived : public Base<T>::Nested { // base class list 中不允许 typename
public:
  explicit Derived(int x) : Base<T>::Nested(x) { // initialization list 中不允许修饰 base class
    typename Base<T>::Nested temp;
  }
};
```

最后一个例子是 typedef typename 连用。

```cpp
tempalte <typename IterT>
void workWithIterator(IterT iter) {
  typedef typename std::iterator_traits<IterT>::value_type value_type; // IterT 如果是 vector<string>::iterator，那么 value_type 就是 string
  value_type temp(*iter);
}
```

## 条款43: 学习处理模板化基类内名称
如果编译期我们有足够的信息来决定哪些信息会如何处理，就可以采用基于 template 的方法。下面是一
个例子：

```cpp
class CompanyA {
public:
  void sendCleartext(const std::string& msg);
  void sendEncrypted(const std::string& msg);
};

class CompanyB {
public:
  void sendCleartext(const std::string& msg);
  void sendEncrypted(const std::string& msg);
};

class MsgInfo {};
template <typename Company>
class MsgSender {
public:
  void sendClear(const MsgInfo& info) {
    std::string msg;
    Company c;
    c.sendCleartext(msg);
  }
  void SendSecret(const MsgInfo& info) {}
}
```

TODO: 

## 条款44: 将与参数无关的代码抽离 templates

## 条款45: 运用成员函数模板接受所有兼容类型
在使用指针时，推荐使用智能指针，它能保障自动删除 heap-based 资源。但是有时候，智能指针并不会
像真实指针那样完成工作，例如 ++ 操作。因为真实指针支持隐式转换，特别是 derived class 转换为 
base class。

```cpp
class Top {};
class Middle : public Top {};
class Bottom : public Middle {};
Top* pt1 = new Middle;
Top* pt2 = new Bottom;
const Top* pct2 = pt1;
```

```cpp
template<typename T>
class SmartPtr {
public:
  explicit SmartPtr(T* realPtr);
};

SmartPtr<Top> pt1 = SmartPtr<Middle>(new Middle);
SmartPtr<Top> pt2 = SmartPtr<Bottom>(new Bottom);
SmartPtr<const Top> pct1 = pt1;
```

上面这两段代码的对比就有这种情况，下面这段代码是不成立的。因为同一个 template 的不同具现体之
间并没有与生俱来的关系，即，不像继承关系那样。为了能够获得我们希望的 SmartPtr class 之间的
转换能力，我们必须将他们明确编写出来。

### Templates 和 泛型编程
如果仅仅通过单一的构造函数来实现，那工程量显然是巨大的。任何新增的一种类型都会需要你通过扩充构
造函数来实现。

我们实际需要的是一个构造模板，这种模板是所谓的 member function templates，其作用是为了 class 
生成 copy 构造函数。

```cpp
template<typename T>
class SmartPtr {
public:
  template<typename U>
  SmartPtr(const SmartPtr<U>& other);
};
```

上面这段代码被称为泛化 copy 构造函数。其中的参数没有声明为 explicit，是因为原始指针类型之间
的转换是隐式转换，无需明白写出的转型动作。

但是，上面这段代码的功能与我们所需的功能有部分冲突，因此我们需要扩充并限制。首先是我们希望安全
的隐式转换发生，而非任意的转换，例如 Base class 转向 derived class 或者 int* 转向 double*。
其次是，我们希望能够提供和 std::auto_ptr 等标准的智能指针相同的方法，例如 get。

```cpp
template<typename T>
class SmartPtr {
public:
  template<typename U>
  SmartPtr(const SmartPtr<U>& other) : heldPtr(other.get()) {}
  T* get() const { return heldPtr; }
private:
  T* heldPtr;
};
```

上面这段既提供了通用方法，又保证了合理的隐式转换的发生。

另外需要注意的一点是，在 class 内声明泛化的 copy 构造函数并不会阻止编译期生成自己的 copy 构
造函数。所以如果你想自己控制 copy 构造的方方面面，就必须同时声明泛化 copy 构造函数和 “正常”
的 copy 构造函数。

## 条款46: 需要类型转换时请为模板定义非成员函数
在条款24中讨论了为什么只有 non-member 函数才有能力 在所有实参身上实施隐式类型转换。但是在模
板化过程中，条款24似乎就不再适用了。

```cpp
template<typename T>
class Rational {
public:
    Rational (const T& numerator = 0, const T& denominator = 1) : _numerator(numerator),
      _denominator(denominator) {}
    const T numerator() const { return _numerator; }
    const T denominator() const { return _denominator; }
private:
    T _numerator;
    T _denominator;
};

template <typename T>
const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs) {
  return Rational(lhs.numerator() * rhs.numerator(), 
    lhs.denominator() * rhs.denominator());
}

Rational<int> oneHalf(1, 2);
Rational<int> result = oneHalf * 2;
```

上述代码无法通过编译，它并没有像非模板那样按照预期运行。这主要是因为，模板的运行过程是先根据传
入的参数进行对模板类型 T 的推算，而在这个过程中并不会进行隐式类型转换。

一种解决这种问题的思路是，利用 friend 来将 non-member 函数声明在 Rational<T> 内，这样就
不再需要根据后面的参数进行推算了，这样也就支持了混合式调用了。

```cpp
template<typename T>
class Rational {
public:
  friend const Rational operator*(const Rational& lhs, const Rational& rhs) {
    return Rational(
      lhs.numerator() * rhs.numerator(),
      lhs.denominator() * rhs.denominator()
    );
  }
};

Rational<int> oneHalf(1, 2);
Rational<int> result = oneHalf * 2;
```

主义上面并没有将 operator* 定义在 Rational 外部，因为那会导致链接失败，这种情况只会发生在 
template 领域中，c 和 OOC 中不会因此而链接失败。为了解决这个问题，我们选在将函数声明和定义
合并在 Rational 内部。这种 friend 的使用方式和往常不同，这里是为了在 class 内部声明一个 
non-member 函数而可选的唯一办法。

写在内部，则意味着也就成为了一个 inline 函数。按照 条款30 所说的那样，如果函数内部过于复杂，
那可以通过调用辅助函数来实现（本案例中函数已经非常简单了，其实不用辅助函数也可以）。

```cpp
template <typename T> class Rational;

template <typename T>
const Rational<T> doMultiply(const Rational<T>& lhs, const Rational<T>& rhs) {
  return Rational<T>(
    lhs.numerator() * rhs.numerator(),
    lhs.denominator() * rhs.denominator()
  );
}

template<typename T>
class Rational {
public:
  friend const Rational operator*(const Rational& lhs, const Rational& rhs) {
    return doMultiply(lhs, rhs);
  }
};
```

作为一个 template，doMultiply 就不再支持混合式乘法了，但是 friend operator* 已经支持混合
式调用了，那也就无需担心 doMultiply 是否支持混合式乘法的问题了。

## 条款47: 请用 traits classes 表现类型信息


## 条款48: 认识 template 元编程

