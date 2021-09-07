# Quickstart: Building with CMake

<https://google.github.io/googletest/quickstart-cmake.html>

- [1. Prerequisites](#1-prerequisites)
- [2. Set up a project](#2-set-up-a-project)
- [3. 创建并运行二进制文件](#3-创建并运行二进制文件)
- [4. 后续步骤](#4-后续步骤)
- [5. 相关参考](#5-相关参考)

## 1. Prerequisites

- 操作系统：Linux, macOS, Windows
- C++ compiler，支持C++11
- [CMake](https://cmake.org/) and a compatible build tool
  - Compatible build tools include [Make](https://www.gnu.org/software/make/), Ninja, and others - see CMake Generators for more information.

如果您尚未安装 CMake，请参阅 [CMake 安装指南](https://cmake.org/install)。

## 2. Set up a project

CMake 使用一个名为`CMakeLists.txt`的文件来配置项目的构建系统。您将使用此文件来建立您的项目并**声明对 GoogleTest 的依赖项**。

首先，为您的项目创建一个目录：

```shell
$ mkdir my_project && cd my_project
```

**接下来，您将创建`CMakeLists.txt`文件并声明对 GoogleTest 的依赖项**。在 CMake 生态系统中有很多表达依赖的方式；在本快速入门中，您将使用 `FetchContentCMake` 模块。为此，在您的项目目录 ( my_project) 中，创建一个名称为CMakeLists.txt并包含以下内容的文件:

```cmake
cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
```

上述配置声明了对从 GitHub 下载的 GoogleTest 的依赖。在上面的例子中，609281088cfefc76f9d0ce82e1ff6c30cc3591e5是要使用的 GoogleTest 版本的 Git 提交哈希；我们建议经常更新哈希以指向最新版本。

**我们建议在项目中使用[master分支](https://github.com/google/googletest)中的最新提交。**

有关如何创建CMakeLists.txt文件的更多信息，请参阅 [CMake 教程](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)。

## 3. 创建并运行二进制文件

将 GoogleTest 声明为依赖项后，您可以在自己的项目中使用 GoogleTest 代码。

例如，在您的 my_project 目录中创建一个命名为hello_test.cc 的文件，内容如下：

```cpp
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
```

要构建代码，请将以下内容添加到CMakeLists.txt文件末尾：

```cmake
enable_testing()

add_executable(
  hello_test
  hello_test.cc
)
target_link_libraries(
  hello_test
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)
```

上述配置启用 CMake 中的测试，声明您要构建的 C++ 测试二进制文件 ( hello_test)，并将其链接到 GoogleTest ( gtest_main)。最后两行使 CMake 的测试运行程序能够使用 [GoogleTest CMake 模块](https://cmake.org/cmake/help/git-stage/module/GoogleTest.html)发现二进制文件中包含的测试。（也可以参考：<https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project>）

现在您可以构建并运行您的测试：

```shell
my_project$ cmake -S . -B build
-- The C compiler identification is GNU 10.2.1
-- The CXX compiler identification is GNU 10.2.1
...
-- Build files have been written to: .../my_project/build

my_project$ cmake --build build
Scanning dependencies of target gtest
...
[100%] Built target gmock_main

my_project$ cd build && ctest
Test project .../my_project/build
    Start 1: HelloTest.BasicAssertions
1/1 Test #1: HelloTest.BasicAssertions ........   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

祝贺！您已经成功地使用 Google 测试构建并运行了测试二进制文件。

## 4. 后续步骤

- 查看 [Primer](https://google.github.io/googletest/primer.html) 开始学习如何编写简单的测试。
- 有关显示如何使用各种 GoogleTest 功能的更多示例，请[参阅代码示例](https://google.github.io/googletest/samples.html)。

## 5. 相关参考

- [GoogleTest CMake 模块](https://cmake.org/cmake/help/git-stage/module/GoogleTest.html)
- [GoogleTest 入门](https://google.github.io/googletest/primer.html)
- [GoogleTest 的通用构建说明](/googletest/README.md)
