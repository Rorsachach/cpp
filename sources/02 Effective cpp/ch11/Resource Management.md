# 资源管理

## 条款13: 以对象管理资源

```cpp
class Investment {};
Investment* createInvestment();
void f() {
    Investment* pInv = createInvestment();

    delete pInv;
}
```

上面这段代码展示了通过工厂函数来提供某个特定的 Investment 对象的过程。看起来没有问题，但实际上当有人开始更改这段代
码时就会产生问题，比如有人在 delete 之前添加 当某些条件满足时 return。这将导致 delete 不会被执行，也就造成了资源
泄露的问题。

**修改的方法：把资源放进对象内，依赖“析构函数自动调用机制”确保资源被释放。**

```cpp
void f() {
    std::auto_ptr<Investment> pInv(createInvestment());
    ...
}
```

这样在运行结束时，auto_ptr 的析构函数会自动删除 pInv。

这种例子展示了这个观点的两个关键想法：
- 获得资源后立即放进管理对象(RAII)
- 管理对象运用自购函数确保资源被释放

需要注意的是别让多个 auto_ptr 同时指向同一个对象。如果是那样，对象会被删除一次以上。而为了预防这个问题，auto_ptr 有
一个不寻常的性质：若通过 copy 构造函数 或 copy assignment 操作符来复制他们，他们会变成 null，而复制所得的指针将
取得资源的唯一拥有权。

```cpp
std::auto_ptr<Investment> pInv1(createInvestment());
std::auto_ptr<Investment> pInv2(pInv1); // 现在 pInv1->nullptr
pInv1 = pInv2; // 现在 pInv2->nullptr
```

### 引用计数型智慧指针
有时候需要允许元素完成正常的复制行为，比如 STL 容器。在这种情况下使用 auto_ptr 就不是最佳选项了。使用 std::shared_ptr
可以允许正常的复制行为。

```cpp
void f() {
    std::shared_ptr<Investment> pInv1(createInvestment());
    std::shared_ptr<Investment> pInv2(pInv1);
}
```

值得注意的是，两者在析构函数中做 delete 而不是 delete[] 操作。因此不要处理动态分配而得来的 array。使用 vector 
或者 string 来代替他们。

本条建议不止包括使用 std::auto_ptr 和 std::shared_ptr 这些资源管理类来管理资源，还建议面向无法被这些管理类管理的
资源通过手动编写自己的资源管理类来进行管理。

## 条款14: 在资源管理类中小心 copying 行为
在条款13中所使用的两个智能指针都旨在管理 heap-based 资源，而有时我们需要管理 non-heap-based 资源时就需要自行撰写
管理对象。

一个例子是为 C API 所提供的 Mutex 互斥对象提供资源管理对象，保证解锁的正常进行。

```cpp
class Lock{
public:
    explicit Lock(Mutex* pm) : mutexPtr(pm) { lock(mutexPtr); }
    ~Lock() { unlock(mutexPtr); }
private:
    Mutex* mutexPtr;
};

Mutex m;
{
    Lock m1(&m); // 进入时加锁，离开此块时 m1 的析构函数自动调用解锁
}
```

看上去一起都好，但是当出现复制操作时问题就会变得复杂。
```cpp
Lock m1(&m);
Lock m2(m1);
```

你可以有4种选择：
- 禁止复制
许多时候对允许 RAII 对象被复制并不合理。这个时候就应该禁止这种行为的发生。条款6给出了方案，使用 private 限定 copy
或者继承含有 private copy 的对象：

```cpp
class Lock : private Uncopyable {
public:
    ...
};
```

- 对底层资源使用 “引用计数法”
有时候，我们希望保有资源直到它的最后一个使用者被销毁。通常使用 std::shared_ptr 来实现引用计数就可以完成这个任务。
需要注意 shared_ptr 的缺省行为是引用为0时删除对象，而我们则需要为 shared_ptr 指定一个删除器 unlock。

```cpp
class Lock{
public:
    explicit Lock(Mutex* pm) : mutexPtr(pm, unlock) { lock(mutexPtr.get()); }
private:
    std::shared_ptr<Mutex> mutexPtr;
};
```

- 复制底部资源
有时候需要针对一份资源拥有任意数量的附件。而资源管理类的唯一理由是，当不再需要某个附件时确保释放它。此时当发生复制时
应该使用深拷贝。

- 转移底部资源的拥有权
某些罕见的场合你可能希望确保永远只有一个 RAII 对象指向一个未加工资源，即使 RAII 对象被复制依然如此。此时，资源的拥
有权会从被复制物转移到目标物。这是 auto_ptr 所奉行的复制意义。

## 条款15: 在资源管理类中