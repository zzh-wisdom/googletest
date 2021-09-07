# 通用构建说明

## 1. Setup

要构建 GoogleTest 和使用它的测试，**您需要告诉构建系统在哪里可以找到它的头文件和源文件**。执行此操作的确切方法取决于您使用的构建系统，并且通常很简单。

## 2. 使用 CMake 构建

GoogleTest 附带一个 CMake 构建脚本 ( [CMakeLists.txt](https://github.com/google/googletest/blob/master/CMakeLists.txt))，可以在各种平台上使用（“C”代表跨平台。）。如果您还没有安装 CMake，您可以从<http://www.cmake.org/> 免费下载 。

CMake 通过生成可在您选择的编译器环境中使用的本机 makefile 或构建项目来工作。**您可以将 GoogleTest 构建为独立项目，也可以将其合并到另一个项目的现有 CMake 构建中**。

### 2.1. 独立的 CMake 项目

将 GoogleTest 构建为独立项目时，典型的工作流程是

```shell
git clone https://github.com/google/googletest.git -b release-1.11.0  # 直接使用master就好
cd googletest        # Main directory of the cloned repository.
mkdir build          # Create a directory to hold the build output.
cd build
cmake ..             # Generate native build scripts for GoogleTest.
```

上面的命令还默认包含 GoogleMock。因此，如果您只想构建 GoogleTest，则应将最后一个命令替换为

```shell
cmake .. -DBUILD_GMOCK=OFF
```

如果您在 *nix（linux/unix） 系统上，您现在应该在当前目录中看到一个 Makefile。只需键入make即可构建 GoogleTest。然后，如果您是系统管理员，则可以简单地安装 GoogleTest。

```shell
make
sudo make install    # Install in /usr/local/ by default
```

如果您使用 Windows 并安装了 Visual Studio，则会创建一个gtest.sln文件和多个.vcproj文件。然后，您可以使用 Visual Studio 构建它们。

在安装了 Xcode 的 Mac OS X 上，.xcodeproj将生成一个文件。

### 2.2. 合并到现有的 CMake 项目中

如果您想在已经使用 CMake 的项目中使用 GoogleTest，最简单的方法是获取已安装的库和头文件。

- 使用find_package（或pkg_check_modules）导入 GoogleTest。例如，如果find_package(GTest CONFIG REQUIRED)成功，您可以将库用作GTest::gtest, GTest::gmock。

更**健壮和灵活**的方法是直接将 GoogleTest 构建为该项目的一部分。这是通过将 GoogleTest 源代码提供给主构建并**使用 CMake 的 add_subdirectory()命令添加它**来完成的。这具有显着的优势，即在 GoogleTest 和项目的其余部分之间使用相同的编译器和链接器设置，因此避免了与使用不兼容库（例如调试/发布）等相关的问题。这在 Windows 上特别有用。可以通过几种不同的方式使得 GoogleTest 的源代码可用于主构建：

- 手动下载 GoogleTest 源代码并将其放置在已知位置。这是最不灵活的方法，并且会使它更难与持续集成系统等一起使用。
- 将 GoogleTest 源代码作为直接副本嵌入到主项目的源代码树中。这通常是最简单的方法，但也是最难更新的。某些组织可能不允许这种方法。
- 将 GoogleTest 添加为 git 子模块或等效项。这可能并不总是可能或适当的。例如，Git 子模块有其自身的优点和缺点。
- **使用 CMake 下载 GoogleTest 作为构建配置步骤的一部分。这种方法没有其他方法的局限性**。

上述最后一个方法是用一小段 CMake 代码实现的，该代码下载 GoogleTest 代码并将其拉入主构建。

只需将下列内容添加到您的CMakeLists.txt：

```cmake
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(example example.cpp)
target_link_libraries(example gtest_main)
add_test(NAME example_test COMMAND example)
```

请注意，由于使用该了`FetchContent_MakeAvailable()`命令，此方法需要 CMake 3.14 或更高版本 。

#### 2.2.1.Visual Studio 动态与静态运行时

默认情况下，新的 Visual Studio 项目动态链接 C 运行时，但 GoogleTest 静态链接它们。这将生成类似于以下内容的错误： gtest.lib(gtest-all.obj) : error LNK2038: mismatch detection for 'RuntimeLibrary': value 'MTd_StaticDebug' does not match value 'MDd_DynamicDebug' in main.obj

GoogleTest 已经为此提供了 CMake 选项： `gtest_force_shared_crt`

启用此选项将使 gtest 也动态链接运行时，并匹配包含它的项目。

### C++ Standard Version

需要支持 C++11 的环境才能成功构建 GoogleTest。确保这一点的一种方法是在顶级项目中指定标准，例如使用`set(CMAKE_CXX_STANDARD 11)`命令。如果这不可行，例如在使用 GoogleTest 进行验证的 C 项目中，则可以通过选项将其添加到 cmake 的`DCMAKE_CXX_FLAGS`选项中来指定。

```shell
cmake -DCMAKE_CXX_COMPILER="clang++" -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++ -U__STRICT_ANSI__"  ../source
```

## 调整 GoogleTest

GoogleTest 可用于多种环境。在某些环境中，默认配置可能无法开箱即用（或可能无法正常工作）。但是，您可以通过在**编译器命令行上**定义控制宏来轻松调整 GoogleTest。通常，**这些宏命名类似为 GTEST_XYZ，您可以将它们定义为 1 或 0 以启用或禁用某个功能**。

## 多线程测试

在pthread库可用的环境中GoogleTest是线程安全的。在`#include "gtest/gtest.h"`后你可以检查宏`GTEST_IS_THREADSAFE`来察觉到是否是线程安全的。如果是安全的，宏被#defined为1，否则宏未定义。

如果 GoogleTest 没有正确检测到 pthread 在您的环境中是否可用，您可以使用

```shell
-DGTEST_HAS_PTHREAD=1
# 或
-DGTEST_HAS_PTHREAD=0
```

> 这里用到的是gcc编译器命令行定义全局宏的功能，即-D选项，可以参考这篇[博客](https://blog.csdn.net/qq_41006901/article/details/103517618)。上面的选项可以简单添加到命令行中。

当 GoogleTest 使用 pthread 时，您可能需要向编译器和/或链接器**添加标志以选择 pthread 库**，否则会出现链接错误。**如果您使用 CMake 脚本，这将为您自动处理**。如果您使用自己的构建脚本，则需要阅读编译器和链接器的手册以找出要添加的标志。

## 作为共享库 (DLL)

GoogleTest 是紧凑的，因此为了简单起见，大多数用户可以将其构建和链接为静态库。如果您愿意，您可以选择使用 GoogleTest 作为共享库（在 Windows 上称为 DLL）。

要将gtest编译为共享库，请添加

```shell
-DGTEST_CREATE_SHARED_LIBRARY=1
```

到**编译器标志**。您还需要告诉链接器生成共享库 - 请参阅链接器手册以了解如何执行此操作。

要编译使用 gtest 共享库的测试，请添加

```shell
-DGTEST_LINKED_AS_SHARED_LIBRARY=1
```

到编译器标志。

注意：虽然在今天使用某些编译器（例如 GCC）时，上述步骤在技术上不是必需的，但如果我们决定提高加载库的速度(它们将来可能会变得必要,请参阅<http://gcc.gnu.org/wiki/Visibility>了解详情）。因此，建议您在使用 GoogleTest 作为共享库时始终添加上述标志。否则 GoogleTest 的未来版本可能会破坏您的构建脚本。

## 避免宏名称冲突

**在 C++ 中，宏不服从命名空间**。因此，如果您#include同时定义两个定义同名宏的两个库，则会发生冲突。如果 GoogleTest 宏与另一个库发生冲突，您可以强制 GoogleTest 重命名其宏以避免冲突。

具体来说，如果 GoogleTest 和其他一些代码都定义了宏 FOO，则可以添加

```shell
-DGTEST_DONT_DEFINE_FOO=1
```

到编译器标志告诉 GoogleTest 将宏的名称从 更改FOO 为GTEST_FOO。当前FOO可以是ASSERT_EQ、ASSERT_FALSE、ASSERT_GE、 ASSERT_GT、ASSERT_LE、ASSERT_LT、ASSERT_NE、ASSERT_TRUE、 EXPECT_FALSE、EXPECT_TRUE、FAIL、SUCCEED、TEST、 或TEST_F。例如，使用-DGTEST_DONT_DEFINE_TEST=1，您需要编写

```cpp
GTEST_TEST(SomeTest, DoesThis) { ... }
```

而不是

```cpp
TEST(SomeTest, DoesThis) { ... }
```

以定义测试。
