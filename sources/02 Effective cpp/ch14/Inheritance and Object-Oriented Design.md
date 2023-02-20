# 继承和面向对象设计

## 条款32: 确定你的 public 继承塑膜出 is-a 关系
以 c++ 进行面向对象编程，最重要的一个规则是: public inheritance 意味着 is-a 的关系。也就
是说，当你令 class D public inheritance class B 时，D 的对象也是一个 B 的对象，但是 B 
的对象不一定是个 D 的对象。

但是这种关系有时候会违反直觉，例如 企鹅 和 鸟 的关系:

```cpp
class Bird {
public:
  virtual void fly();
};

class Penguin : public Bird {};
```

这种继承关系看起来没有问题，但是 Penguin 根本不会飞，这意味着 Bird 类在设计时发生了失误。因
为部分 fly 并不是 Bird 的共性。通常有两种修改方法:

```cpp
void error(const std::string& msg);
class Penguin : public Bird {
public:
  virtual void fly() { error("..."); }
}
```

这种修改方式，当用户尝试调用 penguin.fly 时会产生错误。这并不是最佳的处理方案，因为这会在运
行期产生错误。正如 条款18 所建议的那样，应当将这种错误扼杀在 编译期 内。

```cpp
class Bird {};
class FlyingBird : public Bird {
public:
  virtual void fly();
};

class Penguin : public Bird {};
```

第二种方法是可以按照这种结构修改继承关系。但是这种代码的问题在于，随着维护，未来可能对是否能够 
fly 并不感兴趣，这也意味着这种结构不再适用。当需要进行更改时就会变得十分麻烦。

另外一个例子就是矩形和正方形的关系:

```cpp
class Rectangle {
public:
  virtual void setHeight(int newHeight);
  virtual void setWidth(int newWidth);
  virtual int height() const;
  virtual int int width() const;
};

class Square : public Rectangle {};

void makeBigger(Rectangle& r) {
  int oldHeight = r.height();
  r.setWidth(r.width() + 10);
  assert(r.height() == oldHeight);
}

Square s;
assert(s.width() == s.height());
makeBigger(s);

assert(s.width() == s.height());
```

显然，直观的想法是两个 assert 结果应该相同，因为正方形总是长宽相等的。但是 makeBigger 后，
长宽不相等了，但是后面 assert 又为 true。这显然是不合理的。

is-a 并非唯一的 classes 之间的关系，除此之外还有 has-a 和 is-implemented-in-terms-of 
两种关系。需要了解这些 class 相互关系之间的差异，才能设计好的继承类型。

## 条款33: 避免遮掩继承而来的名称

这个条款主要用来解释作用域的相关内容。

```cpp
int x;
void func() {
  double x;
  std::cin >> x;
}
```

上面这段代码， global x 被 local x 名称遮掩了，因此实际上操作的是 double x。在继承关系里
也会发生这种事情。

```cpp
class Base {
private:
  int x;
public:
  virtual void mf1() = 0;
  virtual void mf2();
  void mf3();
};

class Derived : public Base {
public:
  virtual void mf1();
  void mf4() {
    mf2();
  }
};
```

上述代码中，mf4 调用 mf2，编译器会先在 local 域内查找，没有的话查找 Drived 域，然后 Base 
域，紧接着 namespace，最后 global。因此实际上调用的是 Base::mf2。如果修改一下上面的代码，
就可能发生 遮掩名称 的问题。

```cpp
class Base {
private:
  int x;
public:
  virtual void mf1() = 0;
  virtual void mf1(int);
  virtual void mf2();
  void mf3();
  void mf3(double);
};

class Derived : public Base {
public:
  virtual void mf1();
  void mf3();
  void mf4() {
    mf2();
  }
};

Derived d;
int x;

d.mf1();  // 正确
d.mf1(x); // 错误
d.mf2();  // 正确
d.mf3();  // 正确
d.mf3(x); // 错误
```

上述代码在调用过程中，就会发生名称掩盖。可以看到，当调用 mf1 和 mf3 时都会选择 Derived:: 内
的内容，而没有选择 Base 内的方法。

想要正确运行，有两种方式完成:

1. 使用 using 声明式

```cpp
class Derived : public Base {
public:
  using Base::mf1;
  using Base::mf3;
  virtual void mf1();
  void mf3();
  void mf4() {
    mf2();
  }
};

Derived d;
int x;

d.mf1();  // 正确
d.mf1(x); // 正确
d.mf2();  // 正确
d.mf3();  // 正确
d.mf3(x); // 正确
```

使用 using 声明式来重新声明那些被掩盖的名称，可以实现调用 base function 的功能。

2. 使用转交函数

但是有时候你又不想继承 Base 中的所有函数(这显然不能使用 public 继承)，而是想要继承部分函数。
那么就可以使用 转交函数 来实现。

```cpp
class Derived : private Base {
public:
  virtual void mf1() {
    Base::mf1();
  }
};
```

这样做既不会像 using 那样将所有名称完全暴露，又能继承部分的函数。

## 条款34: 区分接口继承和实现继承
在 public 继承概念的基础上，可以发现它还由两部分组成: 函数接口继承 和 函数实现继承。身为 class 
的设计者有时你会希望 derived class：1. 只继承函数接口，2. 同时继承接口和实现，但是希望能够
覆写，3. 同时继承接口和实现，并且不允许覆写。

```cpp
class Shape {
public:
  virtual void draw() const = 0;
  virtual void error(const std::string& msg);
  int objectID() const;
};

class Rectangle : public Shape {};
class Ellipse : public Shape {};
```

- 成员接口总是会被继承。

Shape 对 derived class 的影响十分深远，正如 public 继承意味着 is-a 关系那样。所有能够施
用于 Shape 的方法，也同样适用 derived class。正是如此，成员接口必须被继承。

- 声明一个 pure virtual 函数的目的是为了让 derived classes 只继承函数接口

在 Shape 中，draw 是一个 pure virtual 函数，它具有两个突出特点: 1. 必须被 继承了他们 的
具象 class 重新声明，2。 abstract class 中通常没有 pure virtual 的定义。这意味着所有继承
自 Shape 的 derived classes 都必须提供一个 draw 函数，但是 Shape 不干涉如何实现。

pure virtual 可以提供定义，但是需要指明 class 才可以调用，例如: Shape::draw()。它可以用
来实现一个机制，为简朴的 impure virtual 函数提供更平常更安全的缺省实现。

- impure virtual 函数的目的是，让 derived classes 继承该函数的接口和缺省实现。

适用 impure virtual 意味着，derived class 的设计者必须支持一个 error 函数，如果不想写，
可以适用 Shape class 的缺省版本。

但是这种 impure virtual 函数的写法很容易因为疏忽而产生问题，特别是在代码维护过程中。

```cpp
class Airport {};
class Airplane {
public:
  virtual void fly(const Airport& destination);
};

void Airplane::fly(const Airport& destination) {
  缺省代码
}

class ModelA: public Airplane {};
class ModelB: public Airplane {};

class ModelC: public Airplane {};
```

上述代码的本意是 ModelA 和 ModelB 采用默认飞行方式，而 ModelC 则适用自己的飞行方式。但是在
程序维护过程中，忘记为 ModelC 实现了新的飞行方式，所以在调用时仍然采用了默认方案。这就产生了
错误。

为了防止这种事情的发生，必须时刻提醒 derived class 的设计者，在 缺省方案 和 自定义方案 中进
行选择。

```cpp
class Airplane {
public:
  virtual void fly(const Airport& destination) = 0;
protected:
  virtual void defaultFly(const Airport& destination);
};
void Airplane::defaultFly(const Airport& destination) {
  ...
}


class ModelA : public Airplane {
public:
  virtual void fly(const Airport& destination) {
    defaultFly(destination);
  }
};

class ModelC : public Airplane {
public:
  virtual void fly(const Airport& destination);
};
void ModelC::fly(const Airport& destination) {
  ...
}
```

通过 pure virtual 来提醒 derived class 设计者必须进行选择，能够很好的防止疏忽产生问题。同
时又通过 defaultFly 来提供默认飞行方式。

上面的代码将接口和缺省实现分开实现了，但是很多人反对这样的事情发生。一种很好的手段就是为 pure 
virtual 函数定义自己的实现，来省去 defaultFly。

```cpp
class Airplane {
public:
  virtual void fly(const Airport& destination) = 0;
};
void Airplane::fly(const Airport& destination) {
  ...
}

class ModelA : public Airplane {
public:
  virtual void fly(const Airport& destination) {
    Airplane::fly(destinatino);
  }
};

class ModelC : public Airplane {
public:
  virtual void fly(const Airport& destination);
};
void ModelC::fly(const Airport& destination) {
  ...
}
```

- 声明 non-virtual 函数的目的是为了令 derived classes 继承函数的接口以及一份强制性实现
non-virtual 函数意味着，不打算在 derived classes 中有不同的行为，它所表现出的不变性凌驾于
特异性之上。

如果你能够履行上面的这些差异，那么你应该能够避免两个错误:

1. 将所有函数声明为 non-virtual。
这让 dervied classes 没有足够的空间进行特化工作。特别是 non-virtual 析构函数。实际上任何 
class 如果打算被用来当作一个 base class，它就会拥有若干个 virtual 函数。

2. 将所有成员函数声明为 virtual
除了部分 interface classes，大多数 class 都会有某些函数就是不该在 derived class 中被重
新定义，那么你应该将这些函数声明为 non-virtual。

## 条款35: 考虑 virtual 函数以外的其他选择

```cpp
class GameCharacter {
public:
  virtual int healthValue() const;
};
```

上面是一个游戏的角色类型，他具有一个 non-pure virtual 函数，用来返回当前生命值。这意味着，
每个不同的角色可以实现不同的 healthValue 方案，同时还具有一个默认的方案。

如何替代 virtual 呢?

### 借由 non-virtual interface 实现 Template Method 模式

```cpp
class GameCharacter {
public:
  int healthValue() const {
    ...
    int retVal = doHealthValue();
    ...
    return retVal;
  }
private:
  virtual int doHealthValue() const {
    缺省代码
  }
};
```

上面代码的思想是，virtual 函数应该几乎总是 private 的。而较好的设计是保留 healthValue 为 
public 成员函数，但是让它成为 non-virtual，并调用一个 private virtual 函数。这种手法被
称为 non-virtual interface (NVI)。它是所谓 Template Method 设计模式的一个独特表现形式。

NVI 手法的优点在于，调用 doHealthValue 的前后可以添加部分处理工作，例如锁定/解锁 mutex，制
造运转日志记录项、验证 class 约束条件、验证函数先决/事后条件等等。

NVI 手法下其实没必要让 virtual 一定得是 private。例如某些函数可能 derived class 必须调用 
base class 的函数，此时就可以适用 protected。而部分要求必须适用 public 修饰的函数，例如
具有多态性质的 base classes 的析构函数，这么依赖就不能使用 NVI 手法了。

### 借由 Function Pointers 实现 Strategy 模式

```cpp
class GameCharacter;
int defaultHealthCalc(const GameCharacter& gc);
class GameCharacter {
public:
  typedef int (*HealthCalcFunc)(const GameCharacter&);
  explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc) : healthFunc(hcf) {}
  int healthValue() const {
    return healthFunc(*this);
  }
private:
  HealthCalcFunc healthFunc;
};

class EvilBadGuy : public GameCharacter {
public:
  explicit EvilBadGuy(HealthCalcFunc hcf = defaultHealthCalc) : GameCharacter(hcf) {}
};

int loseHealthQuickly(const GameCharacter&);
int loseHealthSlowly(const GameCharacter&);

EvilBadGuy ebg1(loseHealthQuickly);
EvilBadGuy ebg1(loseHealthSlowly);
```

通过上面这种方式，借助 Function Pointers 实现 strategy 模式，可以看到有两点好处:
- 同一类型的对象，可以使用不同的运算方法。
- 已知的某个对象，计算过程可以在运行期进行更改。

但是这样设计程序也会带来问题，因为你将一个 member function 替换成为了 non-member function，
这就意味着，如果这个函数需要用到 private member 相关的内容就没有办法了。除非，你尝试通过 firends 
或者为其中一部分提供 public 访问方法，但是这些行为又会降低 GameCharacter 的封装性。因此，
你需要在 non-member funtion pointer 与 封装性 之间进行取舍。

### 借由 function 完成 Strategy

