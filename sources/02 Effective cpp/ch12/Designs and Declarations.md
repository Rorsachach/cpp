# 设计与声明

## 条款18: 让接口容易被正确使用，不易被误用
设计接口的理想原则是，如果客户企图使用某个接口而没有获得他所预期的行为，这个代码就不应该通过编译；如果代码通过了编译，
它的作用就该是客户想要的。这就要求首先必须考虑客户可能做出的错误。

```cpp
class Date {
public:
    Date(int month, int day, int year);
};
```

上面这段代码存在两个问题：
- 客户可能会按照错误的顺序传递参数
- 客户可能传递一个无效的月份或者天数

可以通过导入新类型而获得预防。

```cpp
struct Day {
    explicit Day(int d) : val(d) {}
    int val;
};

struct Month {
    explicit Month(int m) : val(m) {}
    int val;
};

struct Year {
    explicit Year(int y) : val(y) {}
    int val;
};

class Date {
public:
    Date(const Month& month, const Day& day, const Year& year);
};

Date(Month(2), Day(15), Year(2022));
```

这样做就起到了警示作用，同时又限定了传递顺序。

限定了传递顺序后，我们可以进一步对使用的值进行限定。简单的方法是使用 enum 来限定，但是 enum 并不具备类型安全性，例如
enums 可以被用来当作一个 ints 使用。因此可以重新设计一个类。

```cpp
class Month {
public:
    static Month Jan() { return Month(1); }
    static Month Feb() { return Month(2); }
    ...
    static Month Dec() { return Month(12); }
private:
    int val;
    explicit Month(int m) : val(m) {}
};
```

将 Month construct 设置为 private 防止产生新的月份。如果需要选择特定的月份，使用 static function 来完成。

预防客户错误的另一个方法是限制什么事可做，什么不能做。常见的方法是加入 const 限定。例如使用 const 修饰 operator*
的返回类型，可以阻止客户因 用户定义类型而犯错。

另一个一般性准则是：“让 types 容易被正确使用，不容易被误用”。其表示形式是：除非有好理由，否则应该尽量令你的 types 
行为与内置 types 一致。这条准则的内在理由是为了能够提供行为一致的接口。

任何要求客户必须记得做某件事情，就是有着 “不正确使用” 的倾向。例如条款 13 中所提供的 createInvestment 函数。如果
期望客户使用智能指针来接受，实际上是放纵客户产生资源泄露。

```cpp
std::shared_ptr<Investment> createInvestment();
```

上面这种写法杜绝了客户使用过程中忘记 delete 的错误，因为客户必须将其存储在 std::shared_ptr 中。除此之外，也可以提
供特殊的 deleter 来防止客户错误的调用自行定义的 deleter。

```cpp
std::shared_ptr<Investment> createInvestment() {
    std::shared_ptr<Investment> retVal(
        static_cast<Investment*>(0),
        getRidofInvestment()
    );
    retVal = ...; // 令 retVal 指向正确的对象
    return retVal;
}
```

上面这种写法，可以直接返回一个 “将getRidofInvestment” 绑定为删除器的 std::shared_ptr。这样做的另一个好处是它会
自动调用 deleter，因此可以消除潜在的客户错误: cross-DLL problem。这个错误发生于“在 DLL 中被 new 创建的对象，却
在另一个 DLL 内被 delete 释放”。这类行为，在众多平台上会导致运行期错误。而 std::shared_ptr 不会发生这个问题。

## 条款19: 设计 class 犹如设计 type
程序编码的大部分时间都在扩张类型系统。因此需要了解如何设计一个高效的 class (Type)。首先需要了解这需要面对哪些问题：
- 新 type 的对象应该如何被创建和销毁？
- 对象初始化和对象赋值该有什么样的差别？别混淆初始化和赋值，因为他们对应于不同的函数调用。
- 新 type 对象如果被 pass by value 意味着什么？
- 什么是新 type 的合法值？对 class 的成员变量而言，通常只有某些数值集是有效的。
- 你的新 type 需要配合某个继承图系吗？
- 你的新 type 需要什么样的转换？
- 什么样的操作符和函数对此新 type 而言是合理的？
- 什么样的标准函数应该驳回？
- 谁该取用新 type 的成员？
- 什么是新 type 的“未声明接口”？
- 你的新 type 有多么一般化？
- 你真的需要一个新的 type 吗？

## 条款20: 宁以 pass-by-reference-to-const 替换 pass-by-value

```cpp
class Person {
public:
    Person();
    virtual ~Person();

private:
    std::string name;
    std::string address;
};

class Student : public Person {
public:
    Student();
    ~Student();

private:
    std::string schoolName;
    std::string schoolAddress;
};

bool validateStudent(Student s);
Student a;
bool aIsOk = validateStudent(a);
```

上述代码使用 by-value 的方式传值。在这个过程中，首先会调用 Student 的 copy constructor 来对 a 进行赋值，并且在
validateStudent 返回时调用 destructor。其中包含两个 string 的成员变量，因此也会发生对应的复制和销毁操作。同时，
Student 继承自 Person，那么意味着 Person 也会发生复制和销毁，其中包含的两个成员变量也会发生复制和销毁。

可以看到这将经历 6 次构造和析构。使用 pass by reference to const 的方式传递则不会有任何进行调用。

```cpp
bool validateStudent(const Student& s);
```

by-reference 方式也可以避免 slicing 的发生。当一个 derived class 对象被以 by-value 的方式传入到 base class 
中时会发生 slicing，即原有的 derived class 对象的特化性质全被切割掉了。因为这个过程调用的时 base class copy 
constructor。而不会将其余内容进行复制。

```cpp
class Window {
public:
    std::string name() const;
    virtual void display() const;
};

class WindowWithScrollBars : public Window {
public:
    virtual void display() const;
};

void printNameAndDisplay(Window w) {
    std::cout << w.name();
    w.display();
}

WindowWithScrollBars w;
printNameAndDisplay(w);
```

上述代码只会调用 Window::display 而不会调用子类的 display 方法，因为发生了切割。解决 slicing 的方法就是用 by 
reference to const 的方式进行传递。

```cpp
void printNameAndDisplay(const Window& w) {
    std::cout << w.name();
    w.display();
}
```

事实上，by-reference 的底层实现是 pointer 方式，因此如果有个内置类型的对象，by-value 的方式往往比 by-reference 
的方式更加高效。对于 STL 中的迭代器和函数对象，也十分适用。

注意：小型的自定义类型并不意味着和内置类型对象的成本划等号。也就意味着，即使小也不一定适用于 by-value 方式。主要原因
包括两点：
- 编译器可能不这么认为，它可能将 double 放进缓存器中，却不愿意将你用 double 实现的对象放进缓存器。
- type 会变化，随着 type 的不断维护更新可能会变得越来越大。

所以 “pass-by-value 并不昂贵” 的唯一对象就是 **内置类型** 和 **STL的迭代器和函数对象**。

## 条款21: 必须返回对象时，别妄想返回其 reference

```cpp
class Rational {
public:
    Rational (int numerator = 0, int denominator = 1);

private:
    int n, d;
    friend const Rational operator* (const Rational& lhs, const Rational& rhs);
};
```

上述代码中，以 by value 的方式返回计算结果。而上述内容无法修改为返回 reference 因为会出现
意向不到的问题。

```cpp
const Rational& operator* (const Rational& lhs, const Rational& rhs) {
    Rational result (lhs.n * rhs.n, lhs.d * rhs.d);
    return result;
}
```

返回 stack 对象显然是不合理的，因为当脱离当前的 scoop 后，result 被释放，于是返回的 reference 
指向了被销毁的地方。

```cpp
const Rational& operator* (const Rational& lhs, const Rational& rhs) {
    Rational* result = new Rational(lhs.n * rhs.n, lhs.d * rhs.d);
    return *result;
}
```

那么返回一个 heap object 呢？这显然也是不合理的，因为你完全不知道何时去释放掉 new 出来的对
像，甚至有可能你都无法找到那个生成的对象，例如：

```cpp
Rational w, x, y, z;
w = x * y * z; // 连续两次调用，而你无法获取第一次调用产生的指针
```

那么结果就是，当必须要返回新对象时，直接返回新对象就行了。如果成本过高，编译器会想办法进行优化。

## 条款22: 将成员变量声明为 private
首先需要了解为什么成员变量不应该是 public 和 protected 的，然后显然易见应当使用 private。

首先从语法的一致性开始。如果成员变量不是 public，客户就需要使用成员函数来访问成员变量。如此
以来就不再需要区分 成员变量 和 成员函数了，全都按照后者方式访问就行了。

其次，使用函数方式可以更加精确的控制成员变量。而如果成员变量是 public 的，这就意味着任何人都
有权限随时更改对象的成员变量。

```cpp
class AccessLevels {
public:
    int getReadOnly() const { return readOnly; }
    void setReadWrite(int val) { readWrite = val; }
    int getReadWrite() const { return readWrite; }
    void setWriteOnly(int val) { writeOnly = val; } 

private:
    int noAccess;
    int readOnly;
    int readWrite;
    int writeOnly;
};
```

通过精心设计，可以细微的划分访问控制权限。

最后就是封装，如果日后你需要改变某个值的计算方式，使用函数方式进行访问变量，用户完全不会知道
发生了什么改变。这种灵活性有时显得十分重要。

```cpp
class SpeedDataCollection {
public:
    void addValue(int speed);
    double average() const;
};
```

上述代码可以有两个优化方向。一，在对象中维护一个平均速度，当每次调用 average 时直接返回对象。
二，每次被调用时重新计算平均值。

前者需要额外的存储，但是速度十分迅速。而后者计算缓慢却不需要额外的存储空间。因此在内存吃紧，但
很少需要平均值的机器上可以使用前者，内存宽裕但是频繁调用 average 的机器上则可以使用后者。

proteced 限定与 public 在封装上的影响基本类似，后者可以说完全没有封装性，但 protected 的
封装性也十分有限，因为这将影响所有 derived class。

从封装的角度看，其实只有两种访问权限: private(提供封装) 和 其他(不提供封装)。

## 条款23: 宁以 non-member、non-friend 替换 member 函数

```cpp
class WebBrowser {
public:
    void clearCach();
    void clearHistory();
    void removeCoockies();

    void clearEverything();
};

void clearBrowser(WebBrowser& wb) {
    wb.clearCach();
    wb.clearHistory();
    wb.removeCoockies();
}
```

clearEverything 和 clearBrowser 相比之下，后者更好。这点与直观印象相反，面向对象的原则是
数据应该尽可能的被封装，然而与直观相反的是，member 函数带来的封装性要比 non-member 函数的
封装性更低。因为 non-member 函数能提供较大的“包裹弹性”。

对于封装而言，越多东西被封装，代码可改动的内容也就越大，而改动影响到的客户越少。而一个数据越多
函数可以访问，那它的封装性也就越差。

正如条款 22所言，我们要将成员变量声明为 private，而想要访问该变量就需要借助 成员函数 或者 
友元函数。结合前一段所说，选择 non-member non-friend 函数，其封装性也就越好。

有两点内容需要注意：
1. 这个条款用于区分 member 和 non-member non-friend 函数，而非 non-member 函数。
2. 这里的 non-member non-firend 也可以是其他 class 的 member 函数，只要不是 friend

当随着类型的扩充，可能会提供大量的便利函数，而用户通常只需要其中的某一类。比如 WebBrowser 可
能提供了与书签相关的、与打印相关的、与cookie管理相关的内容。这时就可以使用不同的头文件进行管
理。

```cpp
// webbrowser.h 关于 WebBrowser 的定义，以及核心机能
namespace WebBrowserStuff { // 放在一个命名空间中
class WebBrowser {};
// 核心机能函数
}

// webbrowserbookmarks.h
namespace WebBrowserStuff { 
// 书签相关函数
}

// webbrowsercookies.h
namespace WebBrowserStuff { 
// cookie相关函数
}
```