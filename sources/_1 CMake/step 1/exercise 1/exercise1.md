# 构建一个基础的项目
这个基础项目只包含一个单独的源文件，想要完成构建，只需要在 CMakeLists.txt 中加入三个语句就
可以实现。

> [!note]
> CMake 支持 大小写 混用，因此更推荐使用小写方式进行。

项目的文件结构如下。
```txt
exercise 1
  ├── CMakeLists.txt
  └── hello.cpp
```

一个 CMakeLists.txt 文件最少需要下面三个函数进行辅助。
- `cmake_minimum_required(VERSION <min>[...<policy_max>] [FATAL_ERROR])`
设置 cmake 的最低版本限制
- `project(<PROJECT-NAME> [<language-name>...])`
设置项目名称
- `add_executable(<name> [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL] [source1] [source2 ...])`
使用指定的源文件将可执行文件添加到项目中

```cmake
cmake_minimum_required(VERSION 3.10)
project(step1)
add_executable(hello hello.cpp)
```