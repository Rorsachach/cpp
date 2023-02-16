# 实现

## 条款26: 尽可能延后变量定义式的出现时间
---
- 尽可能延后变量定义式的出现。这样做可以增加程序的清晰度并改善程序效率。
---
只要你定义一个变量，而其类型带有一个构造函数或者析构函数，那么当程序控制流到达这个位置时，就会
产生构造成本。离开作用域时你又需要承受析构成本。甚至你从未使用过这个变量。

```cpp
std::string encryptPassword(const std::string& password) {
  using namespace std;
  string encrypted;
  if (password.length() < MinimumPassWordLength) {
    throw logic_error("Password is too short");
  }

  return encrypted;
}
```

上述代码就可能产生一个完全没有被使用的变量 encrypted。通过修改顺序，将 encrypted 的定义式
向后移动，可以避免这种事情的发生。

```cpp
std::string encryptPassword(const std::string& password) {
  using namespace std;
  if (password.length() < MinimumPassWordLength) {
    throw logic_error("Password is too short");
  }
  string encrypted;

  return encrypted;
}
```

但是上述代码效率仍旧不高，因为 encrypted 并没有初始化，这意味着它会先调用 default constructor，
然后再调用 operator=。那前面 default constructor 的工作就前功尽弃了。

```cpp
std::string encryptPassword(const std::string& password) {
  using namespace std;
  if (password.length() < MinimumPassWordLength) {
    throw logic_error("Password is too short");
  }
  string encrypted(password);
  encrypt(encrypted);
  return encrypted;
}
```

这种写法显然更好。这其中包含着延后变量定义式意味着直到能够给变量初值实参为止，再进行定义。但是，
如果遇到循环应该怎么办呢？

```cpp
Widget w;
for (int i = 0; i < n; ++i) {
  w = ...
}
```

```cpp
for (int i = 0; i < n; ++i) {
  Widget w(...);
}
```

这两种方式哪个好。前者的成本更低，因为只需要 n 次赋值操作，但是不易理解和维护，因为 w 所在的
作用域更广，影响更大。而后者成本更高，但是更易维护。因此在选择时，除非 1. 知道赋值成本比构造+
析构更低 2. 代码是效率高度敏感的部分，否则你应该选择下面的做法。

## 条款27: 尽量少做转型动作
在 c++ 中转型会破坏类型系统，而不像 c 一样无可厚非。c++ 提供了 4 种新式的转型方式：
- const_cast<T>(expression): 去除常量性
- dynamic_cast<T>(expression): 安全向下转型，即用来决定某对象是否归属继承体系中的某个类型。(无法通过旧式转型实现)
- reinterpret_cast<T>(expression): 执行低级转型，实际动作取决于编译器，即不可移植。pointer to int
- static_cast<T>(expression): 强迫隐式转型。

推荐使用新式转型，因为很容易在代码中分辨，其次转型动作的目标更窄，更容易被编译器诊断出错误。
转型操作会被编译器编译出对应动作，而非直接更改类型。例如指针，当使用 base class pointer 指
向 derived class 时，指针地址可能并不相同。这种情况下会有偏移量在运行期被施行于 Derived* 
身上。也因此不要做出 “对象在 C++ 中如何布局” 的假设。

错误转型的另外一个场景是，可能写出在其他语言正确在 c++ 中却错误的代码，例如：

```cpp
class Window {
public:
  virtual void onResize() {}
};

class SpecialWindow : public Window {
public:
  virtual void onResize() {
    static_cast<Window>(*this).onResize();
    ...
  }
};
```

上述代码本意是，在子类中先调用父类的 onResize 方法，然后再在当前子类中进行更改。但是这完全不
正确，因为 static_cast 返回的是一个副本，而不是当前对象父类的引用。这意味这上述更改均更改在
了一个 Window 副本中，而当前对象中的 Window 对象却丝毫未动，正确的写法应该如下：

```cpp
class Window {
public:
  virtual void onResize() {}
};

class SpecialWindow : public Window {
public:
  virtual void onResize() {
    Window::onResize();
    ...
  }
};
```

当调用转型时，你很有可能走上了不归路。dynamic_cast更是如此。dynamic_cast 在许多版本中的实
现方案效率都不是很高，因此在注重效率的代码中应该避免使用 dynamic_cast。

通常你使用 dynamic_cast 是因为你认定了该 base class pointer 指向的对象是一个 derived 
class 对象。那想要避免使用 dynamic_cast 有两个方法。

1. 使用容器存储对应的 derived class 指针

```cpp
class Window {};
class SpecialWindow : public Window {
public:
  void blink();
};

typedef std::vector<std::shared_ptr<SpecialWindow>> VPSW;
VPSW winPtrs;

for (VPSW::iterator it = winPtrs.begin(); it != winPtrs.end(); ++it) {
  (*it)->blink();
}
```

这种做法不允许在一个容器内存储不同的 Window 派生类。

2. 通过 base class 接口来处理 “所有可能的各种 Window 派生类”，也就是使用 virtual。

```cpp
class Window {
public:
  virtual void blink() {} // 缺省实现代码并不好
};

class SpecialWindow : public Window {
public:
  virtual void blink() {
    ...
  }
};

typedef std::vector<std::shared_ptr<Window>> VPW;
VPW winPtrs;

for (VPW::iterator it = winPtrs.begin(); it != winPtrs.end(); ++it) {
  (*it)->blink();
}
```

一定要避免的代码是，一连串的 dynamic_casts。

```cpp
for (VPW::iterator it = winPtrs.begin(); it != winPtrs.end(); ++it) {
  (*it)->blink();
  if (SpecialWindow1* psw1 = dynamic_cast<SpecialWindow1*>(iter->get())) {
    ...
  } else if (SpecialWindow2* psw2 = dynamic_cast<SpecialWindow2*>(iter->get())) {
    ...
  } else if (SpecialWindow3* psw3 = dynamic_cast<SpecialWindow3*>(iter->get())) {
    ...
  }
}
```

这样的代码效率极低。总结来看，我们应该尽可能隔离转型动作，通常将它隐藏在某个函数内，函数的接口
会保护调用者不受内部任何动作的影响。

## 条款28: 避免返回 handles 指向对象内部成分
