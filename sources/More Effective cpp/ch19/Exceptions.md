# 异常

## 条款9：利用 destructors 避免泄漏资源
必须要尽量避免使用 “用来操控局部性资源” 的指针，而使用替代方案。

```cpp
class ALA {
public:
  virtual void processAdoption() = 0;
};

class Puppy : public ALA {
public:
  virtual void processAdoption();
};

class Kitten : public ALA {
public:
  virtual void processAdoption();
};
```

假设存在上面的一个类关系，并尝试撰写下面的代码段。其含义在于不断从 istream 中读取数据，并创建
ALA 对象，之后调用 processAdoption 函数，最后删除对象。

```cpp
void processAdoptions(istream &dataSource) {
  while (dataSource) {
    ALA *pa = readALA(dataSource);
    pa->processAdoption();
    delete pa;
  }
}
```

如果 `pa->processAdoption();` 过程发生错误，则会发生非常糟糕的情况。由于processAdoptions
中没有捕获 exception，因此会继续向外抛出异常，这意味着 `delete pa` 操作被忽略。进而会发生资源
泄漏。

解决方案也十分简单，使用 try {} catch {} 语句进行处理，但是这会让该代码变得十分繁琐。

实际上，我们可以采用更简洁的方法，将“一定执行的清理代码”移动到 processAdoptions 中的某个局部
对象的 destructor 中即可。因为局部对象总是会在函数结束时 destruct（除非使用 longjmp）。那么
如何转移呢？

使用一个 “类似指针的对象” 取代指针 pa。这种对象称为 smart pointers。c++ 标准库提供了一个名为
auto_ptr 的 class template。它具有很多特性，在这里的只需要其中一项，对 heap object 进行封
装，以实现局部变量。

```cpp
void processAdoptions(istream &dataSource) {
  while (dataSource) {
    auto_ptr<ALA> pa(readALA(dataSource));
    pa->processAdoption();
  }
}
```

> [!warning]
> 需要注意，auto_ptr 只能适用于 “单一对象”，而面向数组对象则不行。如果需要面向数组，推荐使用
> vector。

### 以一个对象存放“必须自动释放的资源”
使用 auto_ptr 实际上意味着，以一个对象存放 “必须自动释放的资源”，并依赖该对象的 destructor 
释放，而不是放在一个 简单的块 中。这和设计 class 的原则异曲同工，资源的申请和释放应当被放在co-
nstructor/destructor 中，而不是其他成员方法中。

## 条款10：在 constructor 内阻止资源泄漏

## 条款11：禁止 exception 流出 destructor 之外

通常有两种情况 destructor 会被调用：
1. 对象在正常状态下被销毁
2. 对象被 exception 处理机制销毁

第2种情况下，如果 destructor 中再次产生 exception，c++ 则会调用 terminate 函数来终结程序。
一般而言我们无法区分当前到底属于哪种状态，因此应当报社的假设已经产生了 exception。如果这样，我
们必须保证 exceptions 不会从 destructors 中流出。

> [!info]
> uncaught_exception 可以被用来查看当前是否具有未被捕捉的 exception。

除此之外，不让 exceptions 流出 destructors 的另一个好处在于，完整执行所有语句，而不会中途退
出。

```cpp
class Session {
public:
  Session();
  ~Session() { 
    logDestruction(this);
    ...
  }
private:
  static void logCreation(Session *objAddr);
  static void logDestruction(Session *objAddr);
}
```

上面这段代码，如果 logDestruction 产生 exception，则会自动抛出，并且不会执行 ... 中的内容。
如果，当前调用者已经产生了某个 excpetion 则很有可能会自动调用 terminate 函数终结程序。

```cpp
class Session {
public:
  Session();
  ~Session() { 
    try {
      logDestruction(this);
    } catch (...) {}
    ...
  }
private:
  static void logCreation(Session *objAddr);
  static void logDestruction(Session *objAddr);
}
```

在更改后的版本中，使用 try catch 进行 exception 的处理，但是在 catch 后的块中应该尽量避免使
用会产生 exception 的语句。这里选择空，即只捕获不做任何处理。

## 条款12：了解 “抛出一个 exception” 与 “传递一个参数” 或 “调用一个虚函数” 之间的差异
函数参数和 catch 子句的声明语法如出一辙，但是又不完全相同。

### 1. 传递方案
传递方式都采用三种：by value, by reference, by pointer。但是，exception 最好不要使用 by 
pointer 方式进行传递。

### 2. 控制权
函数的调用完成后，控制权会回到调用者手中，而 exception 则完全不同。当你抛出一个 exception 后，
控制权将不会再回到抛出端。

```cpp
istream operator>>(istream &is, Widget &w);
void func() {
  Widget localWidget;
  cin >> localWidget; // 函数调用
  throw localWidget; // 抛出异常
}
```

### 3. 传递副本
在函数调用过程中，如果使用 by reference 方式进行参数传递，则不用像 by value 一样对参数对象进
行复制。但是作为 exception 被抛出时则完全不同，无论使用哪种方式该对象总是会被复制。

其中的原因很简单，exception 的控制权不会被传回到抛出端，这意味着原有抛出端中的所有局部变量都会
被释放掉。如果使用 by reference 方式却只引用原有对象而非副本，则有可能会发生被指对象不存在的问
题。

这种复制行为使用 copy constructor 来执行，因此遵从的是“静态类型”而非“动态类型”。这与c++所默
认的”复制动作永远是以对象的静态类型为本“是一致的。

```cpp
class Widget {};
class SpecialWidget : public Widget {};
void func() {
  SpecialWidget localSpecialWidget;
  Widget &w = localSpecialWidget;
  throw w; // 这里抛出的对象类型是 Widget 而非 SpecialWidget
}
```

### 4. 如何在 catch 语句块内传播 exceptions

```cpp
catch (Widget &w) {
  throw;
}

catch( Widget &w) {
  throw w;
}
```

这两种块内传播方式的区别在于是否进行副本传递。前者非常简单，重新抛出当前的 exception。这意味着，
不需要复制一个副本，同时如果在此之前抛出者抛出的是 SpecialWidget，该子语句也不会从新抛出 Widget。
而后者则不同，后者是新抛出一个 exception，因此无论如何该子语句都会向外抛出一个 Widget 类型的
exception。

### 5. reference-to-const 需不需要
在函数传递中，不允许将一个 临时对象 传递给 non-const reference 参数，而 exception 则允许。

### 6. by value 还是 by reference
在复制 exception objects 的过程中 by value 实际需要创建两个副本：1. 任何 exception 都会
产生的临时对象。2. 将临时对象复制到 catch 中的参数中。

而使用 by reference 则只需要创建一个副本，即最后将临时对象只需要以引用的方式传递给 catch 中的
参数，而非再次复制。

### 7. 类型转换
在函数调用过程中，可能会出现隐式类型转换的情况，例如：

```cpp
double sqrt(double);

int i = 4;
double ans = sqrt(i); // 这里的 i 被转换为了 double 类型
```

可以看到上面的调用过程，类型会发生变化。但是在 exception 的抛出过程中却完全不会发生。

```cpp
#include <iostream>

using namespace std;

int main() {
  int i = 0;
  try {
    if (i == 0) {
      throw i;
    }
  }
  catch (double d) {
    cout << d << endl;
  }

  return 0;
}
```

exception 与 catch 子句匹配的过程中允许两种类型转换的发生：
- 继承架构中的类型转换：针对 base class exception 编写的 catch 子句可以用来处理 derived class
- 针对 runtime_errors 编写的 catch 子句，可以用来捕捉 range_error 和 overflow_error 的 exceptions

使用 `catch (const void *)` 可以用来捕捉任何指针类型的 exception

### 8. first fit 匹配机制
catch 子句的捕获方式是按照顺序进行匹配的，因此当 catch base class 和 derived class 时，后
者可能会因为循序问题被忽略。

```cpp
class Widget {};
class SpecialWidget : public Widget {};

try {}
catch (Widget &w) {}
catch (SpecialWidget &w) {} // 永远不会执行，因为一定会被上一个 catch 捕获
```

可见 exception 的处理只与顺序有关系，因此是 first fit 的。而虚函数的调用则是依据”调用者的动
态类型“，因此可以说是 best fit 的。

## 条款13：以 by reference 方式捕捉 exception
理论上可以使用 by pointer，by value, by reference 中的任一个方式来捕捉 exception，但是
前两者都存在部分缺陷。

### 1. by pointer

```cpp
void func() {
  static exception ex;
  throw &ex;
}

void doSomething() {
  try {
    func();
  }
  catch (exception *ex) {}
}
```

上面给出了一个合理的写法，需要注意的是 `static exception ex;`。因为控制权从抛出端丢失后，局
部变量将会被销毁，因此为了在 doSomething 中仍然可以通过指针方式获取 ex，则需要使用 static 关
键字。

如果使用 heap-object 呢？

```cpp
void func() {
  throw new exception();
}

void doSomething() {
  try {
    func();
  }
  catch (exception *ex) {}
}
```

这就引入了另一个问题，如果使用 by pointer 方式来捕获 exception，是否需要在处理时进行资源清理。
如果使用的 static-non-heap object，调用 delete 则会发生错误。如果使用的 heap object 而
没有调用 delete 则会出现内存泄漏的问题。

这个问题无法解决，也就意味着应该避免使用 by pointer 的方式来捕获 exception。

### 2. by value
by value 的方法可以解决上面这个问题，但是存在两点缺点。

首先，正如 条款12 中所描述的那样，by value 的方式，exception object 会被复制两次。

其次，会出现切割问题。throw exception 的过程一定会进行一次复制操作，同时复制过程默认使用的是
“静态类型”。那么，原有的 derived object 被抛出时，可能会因为 catch 子句捕获 base object，
而发生切割。

```cpp
class Widget {
public:
  int a;
};

class SpecialWidget : public Widget {
public:
  int b;
};

void func () {
  SpecialWidget w;
  w.a = 1;
  w.b = 2;
  throw w;
}

int main () {
  try {
    func();
  }
  catch(Widget w) {
    SpecialWidget *sw = (SpecialWidget *) &w;
    cout << sw->b << endl;
  }
}
```

可以发现 sw->b 的值被改变了，准确的说是不存在了。

### 3. by reference
by reference 的方式则完美的解决了 by value 的问题，只需要复制一次，且不会发生切割问题。可以
将上面的代码进行修改，可以发现 sw->b 仍然存在。

```cpp
int main () {
  try {
    func();
  }
  catch(Widget &w) {
    SpecialWidget *sw = (SpecialWidget *) &w;
    cout << sw->b << endl;
  }
}
```

可见 catch exceptions by reference!

## 条款14：明智运用 exception specification
使用 exception specification 能够让代码更加漂亮，同时有时也能指导编译器在编译期间检测不一致
的行为。但是有必要仔细思考一下应该注意那些问题。

```cpp
void func() throw(int);
```

### 1. 不应该将 templates 和 exception specification 混用
由于 c++ 中没有任何方法可以指导一个 template 的类型参数可能抛出什么 exceptions，因此不要混用。

### 2. 调用关系之间步调要统一
如果 A 函数调用了 B 函数，而 B 函数没有 exception specification，那么 A 函数也不应当提供 
exception specification。

需要额外注意的是 callback 函数。

```cpp
typedef void (*CallBackPtr)(int eventXLocation, int eventYLocation,
  void *dataToPassBack);

class CallBack {
public:
  CallBack(CallBackPtr fPtr, void *dataToPassBack) : func(fPtr), 
    data(dataToPassBack) {}
  void makeCallBack(int eventXLocation, int eventYLocation) const throw();

private:
  CallBackPtr func;
  void *data;
};
```

因为 typedef 定义时没有添加 throw()，因此makeCallBack调用func 可能违反 exception spec-
ification。

在函数指针传递之际检验 exception specifications 是晚些加入的一个新特性，如果编译器不支持就
只能自求多福。

### 3. 处理“系统”可能抛出的 exceptions
尽管在编码过程中，你频繁使用了 exception specifications 来进行标记，但当你使用标准库函数时
仍然可能产生你所未预料到的 exceptions。此时就需要去处理这些可能的异常。

如果你认为非预期的 exceptions 是件不且实际的事情，可以利用自行设计的 exception class 来进
行取代。

```cpp
class UnexpectedException {};

void convertUnexpected() { // 任何非预期的 expection 都抛出 UnexpectedException 对象
  throw UnexpectedException();
}

set_unexpected(convertUnexpected); // 取代默认的 unexpected 函数
```

上述代码可以被简化，因为当非预期 exception 被重新抛出时，会被 bad_exception 取代。因此可以
进行如下修改：

```cpp
void convertUnexpected() { // 任何非预期的 expection 都抛出 UnexpectedException 对象
  throw; // 这样默认会得到一个 bad_exception
}
```

可以看到，exception sepcification 可能带来了更多的麻烦，当产生未预料到的 exception 很可能
造成整个程序的停止，甚至是调用者已经做好处理 exception 的准备的情况下。

```cpp
class Session {
public:
  ~Session();
private:
  static void log(Session *objAddr) throw(); // 预计不会抛出任何异常
}

Session::~Session() {
  try {
    log(this);
  } catch(...) {} // 尽量避免空指针而导致的程序终结
}
```

很不幸，上述代码仍然可能会发生终结，因为 unexpected 函数会被调用，而不会进入 catch 子句中。

所以，慎重使用 exception sepcification。

## 条款15：了解异常处理 (exception handling) 的成本

异常处理过程，需要进行大量的记录工作，了解其中的过程尽量减少付出的成本。

通常如果从未使用任何 exception 处理机制，程序较小，执行速度也块。如果全部程序都没有用到 exceptions
处理相关的内容，可以在编译器中设置 放弃支持 exception 来降低成本。

try 会产生额外的成本，尽力那个避免非必要的 try 语句块。

exception specifications 产生的成本与 try 语句块近似。

通常抛出 exception 而产生的函数返回，其速度可能比正常情况下慢 3 个数量级。

> [!tip]
> 在写 exception 处理过程时对成本需要进行考虑，但是并非斤斤计较。在非用不可的地方使用 try 和 
> exception soecification，在真正异常的情况下再抛出 exception。

