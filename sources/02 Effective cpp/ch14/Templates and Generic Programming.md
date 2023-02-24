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

## 条款43: 学习处理模板化积累内名称
