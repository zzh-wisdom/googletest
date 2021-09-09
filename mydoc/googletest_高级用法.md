# Advanced googletest Topics

原文: <https://google.github.io/googletest/advanced.html>

- [1. More Assertions](#1-more-assertions)
  - [1.1. 更好打印错误消息的谓词断言](#11-更好打印错误消息的谓词断言)
  - [1.2. 浮点谓词格式函数](#12-浮点谓词格式函数)
  - [1.3. 使用 gMock 匹配器进行断言](#13-使用-gmock-匹配器进行断言)
  - [1.4. 更多字符串断言](#14-更多字符串断言)
  - [1.5. 类型断言](#15-类型断言)
  - [1.6. 断言放置](#16-断言放置)
- [2. 跳过测试执行](#2-跳过测试执行)
- [3. 教 googletest 如何打印你的值 ※](#3-教-googletest-如何打印你的值-)
- [4. Death Tests](#4-death-tests)
- [5. 正则表达式语法](#5-正则表达式语法)
- [6. 在子程序中使用断言 ※](#6-在子程序中使用断言-)
  - [6.1. 为断言添加跟踪](#61-为断言添加跟踪)
  - [6.2. 传播致命的失败](#62-传播致命的失败)
- [7. 记录附加信息](#7-记录附加信息)
- [8. 在同一测试套件中的测试之间共享资源 ※](#8-在同一测试套件中的测试之间共享资源-)
- [9. 全局Set-Up和Tear-Down ※](#9-全局set-up和tear-down-)
- [10. 值参数化测试 ※](#10-值参数化测试-)
  - [10.1. 如何编写值参数化测试](#101-如何编写值参数化测试)
  - [参数生成器](#参数生成器)
  - [10.2. 创建值参数化抽象测试](#102-创建值参数化抽象测试)
  - [10.3. 指定值参数化测试参数的名称](#103-指定值参数化测试参数的名称)
- [11. 类型测试](#11-类型测试)
- [12. 类型参数化测试](#12-类型参数化测试)
- [13. 测试私有代码](#13-测试私有代码)
- [14. “捕捉”失败](#14-捕捉失败)
- [15. 以编程方式注册测试](#15-以编程方式注册测试)
- [16. 获取当前测试的名称](#16-获取当前测试的名称)
- [17. 通过处理测试事件扩展 googletest](#17-通过处理测试事件扩展-googletest)
- [18. 运行测试程序：高级选项 ※](#18-运行测试程序高级选项-)

本文档将向您展示更多断言以及如何构建复杂的故障消息、传播致命故障、重用和加速测试装置，以及如何在测试中使用各种**标志**。

## 1. More Assertions

本节涵盖了一些不太常用但仍然很重要的断言。

- 明确的成功和失败: <https://google.github.io/googletest/reference/assertions.html#success-failure>
- 异常断言: <https://google.github.io/googletest/reference/assertions.html#exceptions>

### 1.1. 更好打印错误消息的谓词断言

尽管 googletest 有一组丰富的断言，但它们永远不可能是完整的，因为预测用户可能遇到的所有场景是不可能的（也不是一个好主意）。
因此，有时用户必须使用 EXPECT_TRUE() 来检查复杂的表达式，因为缺少更好的宏。
这有一个问题，即**没有向您显示表达式各部分的值，从而难以理解出了什么问题**。
作为一种变通方法，一些用户选择自己构建失败消息，将其流式传输到 EXPECT_TRUE() 中。
然而，这很尴尬，尤其是当表达式有副作用或评估成本高时。

googletest 为您提供了三种不同的选项来解决这个问题：

1. **使用现有的布尔函数**

即谓词断言：[EXPECT_PRED*](https://google.github.io/googletest/reference/assertions.html#EXPECT_PRED)

出错时可以打印函数/算子的名称

2. **使用返回 AssertionResult 的函数**

虽然 EXPECT_PRED*() 对于快速工作很方便，但语法并不令人满意：您必须为不同的参数使用不同的宏，而且感觉更像是 Lisp 而不是 C++。 
::testing::AssertionResult 类解决了这个问题。

AssertionResult 对象表示断言的结果（无论是成功还是失败，以及相关的消息）。
您可以使用以下**工厂函数**之一创建 AssertionResult：

```cpp
namespace testing {

// Returns an AssertionResult object to indicate that an assertion has
// succeeded.
AssertionResult AssertionSuccess();

// Returns an AssertionResult object to indicate that an assertion has
// failed.
AssertionResult AssertionFailure();

}
```

然后，您可以**使用 << 运算符将消息流式传输到 AssertionResult 对象**。

要在布尔断言（例如 EXPECT_TRUE()）中提供更具可读性的消息，请编写一个返回 AssertionResult 而不是 bool 的谓词函数。
例如，如果您将 IsEven() 定义为：

```cpp
testing::AssertionResult IsEven(int n) {
  if ((n % 2) == 0)
    return testing::AssertionSuccess();
  else
    return testing::AssertionFailure() << n << " is odd";
}
```

而不是：

```cpp
bool IsEven(int n) {
  return (n % 2) == 0;
}
```

失败的断言 `EXPECT_TRUE(IsEven(Fib(4)))` 将打印：

```cpp
Value of: IsEven(Fib(4))
  Actual: false (3 is odd)
Expected: true
```

而不是像后者那样不透明：

```cpp
Value of: IsEven(Fib(4))
  Actual: false
Expected: true
```

如果您还需要 `EXPECT_FALSE` 和 `ASSERT_FALSE` 中的信息性消息（Google 代码库中三分之一的布尔断言都是基于否定的，因为成功情况会使得谓词变慢），则可以提供成功消息：

```cpp
testing::AssertionResult IsEven(int n) {
  if ((n % 2) == 0)
    return testing::AssertionSuccess() << n << " is even";
  else
    return testing::AssertionFailure() << n << " is odd";
}
```

然后语句 EXPECT_FALSE(IsEven(Fib(6))) 将打印

```cpp
  Value of: IsEven(Fib(6))
     Actual: true (8 is even)
  Expected: false
```

3. **使用谓词格式化程序**

[EXPECT_PRED_FORMAT*](https://google.github.io/googletest/reference/assertions.html#EXPECT_PRED_FORMAT)

### 1.2. 浮点谓词格式函数

一些浮点运算很有用，但并不常用。
为了避免新宏的激增，我们将它们作为谓词格式函数提供，可以在谓词断言宏 EXPECT_PRED_FORMAT2 中使用，例如：

```cpp
EXPECT_PRED_FORMAT2(testing::FloatLE, val1, val2);
EXPECT_PRED_FORMAT2(testing::DoubleLE, val1, val2);
```

上面的代码验证了 val1 小于或近似等于 val2。

### 1.3. 使用 gMock 匹配器进行断言

请参阅断言参考中的 [EXPECT_THAT](https://google.github.io/googletest/reference/assertions.html#EXPECT_THAT)。

### 1.4. 更多字符串断言

您可以使用带有 EXPECT_THAT 的 gMock 字符串匹配器来执行更多字符串比较技巧（子字符串、前缀、后缀、正则表达式等）。
例如，

```cpp
using ::testing::HasSubstr;
using ::testing::MatchesRegex;
...
ASSERT_THAT(foo_string, HasSubstr("needle"));
EXPECT_THAT(bar_string, MatchesRegex("\\w*\\d+"));
```

### 1.5. 类型断言

```cpp
::testing::StaticAssertTypeEq<T1, T2>();
```

断言 T1 和 T2 类型相同。
如果断言得到满足，该函数不执行任何操作。
如果类型不同，函数调用将无法编译，编译器错误消息会说 T1 和 T2 不是同一类型，并且很可能（取决于编译器）向您显示 T1 和 T2 的实际值。
这主要在**模板代码**中很有用。

警告：当在类模板或函数模板的成员函数中使用时，StaticAssertTypeEq() 仅在函数被实例化时有效。
例如，给定：

```cpp
template <typename T> class Foo {
 public:
  void Bar() { testing::StaticAssertTypeEq<int, T>(); }
};
```

对于代码：

```cpp
void Test1() { Foo<bool> foo; }
```

不会产生编译器错误，因为 Foo::Bar() 从未真正实例化。相反，您需要：

```cpp
void Test2() { Foo<bool> foo; foo.Bar(); }
```

以产生编译器错误。

### 1.6. 断言放置

您可以在任何 C++ 函数中使用断言。
特别是，它不必是处于测试fixture类的方法。
一个限制是产生**致命失败的断言（FAIL* 和 ASSERT_*）只能在返回 void 的函数中使用**。
这是 Google 不使用异常的结果。通过将它放在一个非 void 函数中，你会得到一个令人困惑的编译错误，比如"error: void value not ignored as it ought to be" or "cannot initialize return object of type 'bool' with an rvalue of type 'void'" or "error: no viable conversion from 'void' to 'string'".

如果您需要在返回非 void 的函数中使用致命断言，一种选择是让函数的返回值携带则 out 参数中。
例如，您可以将 `T2 Foo(T1 x)` 重写为 `void Foo(T1 x, T2* result)`。
即使函数过早返回，您也需要确保 \*result 包含一些合理的值。
由于该函数现在返回 void，您可以在其中使用任何断言。

如果不能选择更改函数的类型，则应仅使用生成非致命故障的断言，例如 `ADD_FAILURE*` 和 `EXPECT_*`。

> 注意：根据 C++ 语言规范，构造函数和析构函数不被视为返回空值的函数，因此您不能在其中使用致命断言；
> 如果你尝试，你会得到一个编译错误。相反，要么调用 abort 并使整个测试可执行文件崩溃，要么将致命断言放在 SetUp/TearDown 函数中；请参阅构造函数/析构函数与设置/拆卸

## 2. 跳过测试执行

与断言 SUCCEED() 和 FAIL() 相关，您可以使用 GTEST_SKIP() 宏防止在运行时进一步执行测试。
当您需要在运行时检查被测系统的先决条件并**以有意义的方式跳过测试**时，这很有用。

GTEST_SKIP() 可用于单个测试用例或派生自 ::testing::Environment 或 ::testing::Test 的类的 SetUp() 方法。例如：

```cpp
TEST(SkipTest, DoesSkip) {
  GTEST_SKIP() << "Skipping single test";
  EXPECT_EQ(0, 1);  // Won't fail; it won't be executed
}

class SkipFixture : public ::testing::Test {
 protected:
  void SetUp() override {
    GTEST_SKIP() << "Skipping all tests for this fixture";
  }
};

// Tests for SkipFixture won't be executed.
TEST_F(SkipFixture, SkipsOneTest) {
  EXPECT_EQ(5, 7);  // Won't fail
}
```

与断言宏一样，您可以将自定义消息流式传输到 GTEST_SKIP()。

## 3. 教 googletest 如何打印你的值 ※

当诸如 EXPECT_EQ 之类的测试断言失败时，googletest 会**打印参数值**以帮助您进行调试。它使用用户可扩展的值printer来完成此操作。

这台打印机知道如何打印内置 C++ 类型、原生数组、STL 容器和任何支持 << 运算符的类型。对于其他类型，**它会打印值中的原始字节**，并希望您的用户能够弄清楚。

如前所述，打印机是可扩展的。这意味着您可以教它在打印您的特定类型方面做得更好，而不是转储字节。为此，请为您的类型定义 <<：

```cpp
#include <ostream>

namespace foo {

class Bar {  // We want googletest to be able to print instances of this.
...
  // Create a free inline friend function.
  friend std::ostream& operator<<(std::ostream& os, const Bar& bar) {
    return os << bar.DebugString();  // whatever needed to print bar to os
  }
};

// If you can't declare the function in the class it's important that the
// << operator is defined in the SAME namespace that defines Bar.  C++'s look-up
// rules rely on that.
std::ostream& operator<<(std::ostream& os, const Bar& bar) {
  return os << bar.DebugString();  // whatever needed to print bar to os
}

}  // namespace foo
```

有时，这可能不是一种选择：您的团队可能认为为 Bar 使用 << 操作符是一种糟糕的风格，或者 Bar 可能已经有一个 << 操作符不能满足您的要求（并且您无法更改它）。如果是这样，您可以改为定义一个 `PrintTo()` 函数，如下所示：

```cpp
#include <ostream>

namespace foo {

class Bar {
  ...
  friend void PrintTo(const Bar& bar, std::ostream* os) {
    *os << bar.DebugString();  // whatever needed to print bar to os
  }
};

// If you can't declare the function in the class it's important that PrintTo()
// is defined in the SAME namespace that defines Bar.  C++'s look-up rules rely
// on that.
void PrintTo(const Bar& bar, std::ostream* os) {
  *os << bar.DebugString();  // whatever needed to print bar to os
}

}  // namespace foo
```

**如果您同时定义了 << 和 PrintTo()，则在涉及 googletest 时将使用后者**。这允许您自定义值在 googletest 输出中的显示方式，而不会影响依赖于其 << 运算符行为的代码。

如果您想自己**使用 googletest 的值打印机打印值 x，只需调用 ::testing::PrintToString(x)，它返回一个 std::string：**

```cpp
vector<pair<Bar, int> > bar_ints = GetBarIntVector();

EXPECT_TRUE(IsCorrectBarIntVector(bar_ints))
    << "bar_ints = " << testing::PrintToString(bar_ints);
```

## 4. Death Tests

在许多应用程序中，如果不满足某个条件，断言可能会导致应用程序失败。这些确保程序处于已知良好状态的一致性检查会在某些程序状态损坏后尽早失败。如果断言检查了错误的条件，则程序可能会以错误的状态继续运行，这可能导致内存损坏、安全漏洞或更糟。因此，**测试此类断言语句是否按预期工作非常重要**。

**由于这些先决条件检查会导致进程终止，因此我们称此类测试为死亡测试**。更一般地说，任何检查程序是否以预期方式**终止**（除非抛出异常）的测试也是死亡测试。

请注意，如果一段代码抛出异常，出于死亡测试的目的，我们不会将其视为“死亡”，因为代码的调用者可以捕获异常并避免崩溃。
如果要验证代码抛出的异常，请参阅异常断言。

[死亡断言](https://google.github.io/googletest/reference/assertions.html#death)
如何写一个死亡测试
死亡测试命名

## 5. 正则表达式语法

1. [POSIX extended regular expression](http://www.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap09.html#tag_09_04)

[Wikipedia](http://en.wikipedia.org/wiki/Regular_expression#POSIX_Extended_Regular_Expressions)

2. 简单的正则表达式

## 6. 在子程序中使用断言 ※

注意：如果您想在子程序中放置一系列测试断言以检查复杂条件，请考虑使用**自定义 GMock 匹配器**。
这使您可以在出现故障时提供更具可读性的错误消息，并避免出现下面描述的所有问题。

### 6.1. 为断言添加跟踪

如果从多个地方调用一个测试子例程，当它内部的断言失败时，可能很难判断失败来自子例程的哪个调用。您可以使用额外的日志记录或自定义失败消息来缓解此问题，但这通常会使您的测试变得混乱。更好的解决方案是使用 `SCOPED_TRACE` 宏或 `ScopedTrace` 实用程序：

```cpp
SCOPED_TRACE(message);
ScopedTrace trace("file_path", line_number, message);
```

其中 message 可以是任何可流式传输到 std::ostream 的东西。 
**SCOPED_TRACE 宏将导致在每个失败消息中添加当前文件名、行号和给定消息**。 ScopedTrace 在参数中接受显式文件名和行号，这对于编写测试助手很有用。当控件离开当前词法范围时，效果将被撤消。

```cpp
10: void Sub1(int n) {
11:   EXPECT_EQ(Bar(n), 1);
12:   EXPECT_EQ(Bar(n + 1), 2);
13: }
14:
15: TEST(FooTest, Bar) {
16:   {
17:     SCOPED_TRACE("A");  // This trace point will be included in
18:                         // every failure in this scope.
19:     Sub1(1);
20:   }
21:   // Now it won't.
22:   Sub1(9);
23: }
```

可能会导致这样的消息：

```shell
path/to/foo_test.cc:11: Failure
Value of: Bar(n)
Expected: 1
  Actual: 2
Google Test trace:
path/to/foo_test.cc:17: A

path/to/foo_test.cc:12: Failure
Value of: Bar(n + 1)
Expected: 2
  Actual: 3
```

如果没有跟踪，就很难知道两次失败分别来自哪个 Sub1() 调用。 
（您可以在 Sub1() 中的每个断言中添加一个额外的消息来指示 n 的值，但这很乏味。）

使用 SCOPED_TRACE 的一些提示：

- 对于合适的消息，通常在子例程的开头使用 SCOPED_TRACE 就足够了，而不是在每个调用点。
- 在循环内调用子例程时，将**循环迭代器作为 SCOPED_TRACE 中消息的一部分**，这样您就可以知道失败来自哪个迭代。
- 有时，跟踪点的行号足以识别子例程的特定调用。在这种情况下，您不必为 SCOPED_TRACE 选择唯一的消息。您可以简单地使用“”。
- 当外部作用域中有一个SCOPED_TRACE时，您可以在内部作用域中使用 SCOPED_TRACE。在这种情况下，所有活动跟踪点都将包含在失败消息中，以**相反的顺序**遇到它们。
- 跟踪转储在 Emacs 中是可点击的——在行号上点击回车，你将被带到源文件中的那一行！

### 6.2. 传播致命的失败

使用 ASSERT_* 和 FAIL* 时的一个常见陷阱是不明白**当它们失败时，它们只会中止当前函数，而不是整个测试**。例如，以下测试将出现段错误：

```cpp
void Subroutine() {
  // Generates a fatal failure and aborts the current function.
  ASSERT_EQ(1, 2);

  // The following won't be executed.
  ...
}

TEST(FooTest, Bar) {
  Subroutine();  // The intended behavior is for the fatal failure
                 // in Subroutine() to abort the entire test.

  // The actual behavior: the function goes on after Subroutine() returns.
  int* p = nullptr;
  *p = 3;  // Segfault!
}
```

为了缓解这种情况，googletest 提供了三种不同的解决方案。
您可以使用异常，(ASSERT|EXPECT)_NO_FATAL_FAILURE 断言或 HasFatalFailure() 函数。它们在以下两个小节中进行了描述。

1. **通过异常在子例程中断言**

以下代码可以将 ASSERT 失败变成异常：

```cpp
class ThrowListener : public testing::EmptyTestEventListener {
  void OnTestPartResult(const testing::TestPartResult& result) override {
    if (result.type() == testing::TestPartResult::kFatalFailure) {
      throw testing::AssertionException(result);
    }
  }
};
int main(int argc, char** argv) {
  ...
  testing::UnitTest::GetInstance()->listeners().Append(new ThrowListener);
  return RUN_ALL_TESTS();
}
```

如果有其他监听器的话，这个监听器应该在其他监听器之后添加，否则他们不会看到失败的 OnTestPartResult。

2. **断言子程序**

如上所示，如果您的测试调用了一个包含 ASSERT_* 失败的子程序，则该测试将在子程序返回后继续进行。这可能不是您想要的。

**人们通常希望致命的失败像异常一样传播**。为此，googletest 提供了以下宏：

```cpp
ASSERT_NO_FATAL_FAILURE(statement);
EXPECT_NO_FATAL_FAILURE(statement);
```

从而可以确认statement不在**当前线程**产生任何致命错误。

3. **检查当前测试中的故障**

如果当前测试中的断言遭遇致命失败，::testing::Test 类中的 **HasFatalFailure()** 将返回 true。
这允许函数在子例程中捕获致命故障并提前返回。

```cpp
class Test {
 public:
  ...
  static bool HasFatalFailure();
};
```

基本上模拟抛出异常行为的典型用法是：

```cpp
TEST(FooTest, Bar) {
  Subroutine();
  // Aborts if Subroutine() had a fatal failure.
  if (HasFatalFailure()) return;

  // The following won't be executed.
  ...
}
```

如果 HasFatalFailure() 在 TEST() 、 TEST_F() 或测试 fixture 之外使用，则必须添加 `::testing::Test::` 前缀，如下所示：

```cpp
if (testing::Test::HasFatalFailure()) return;
```

类似地，如果当前测试至少有一个**非致命失败**，则 `HasNonfatalFailure()` 返回 true；如果当前测试至少有任何一种失败，`HasFailure()` 返回 true。

## 7. 记录附加信息

在您的测试代码中，您可以调用 `RecordProperty("key", value)` 来记录附加信息，其中 value 可以是**字符串或整数**。如果您指定一个key，则为key记录的最后一个值将被发送到 [XML 输出](https://google.github.io/googletest/advanced.html#generating-an-xml-report)。例如，测试

```cpp
TEST_F(WidgetUsageTest, MinAndMaxWidgets) {
  RecordProperty("MaximumWidgets", ComputeMaxUsage());
  RecordProperty("MinimumWidgets", ComputeMinUsage());
}
```

将像这样输出 XML：

```xml
  ...
    <testcase name="MinAndMaxWidgets" status="run" time="0.006" classname="WidgetUsageTest" MaximumWidgets="12" MinimumWidgets="9" />
  ...
```

- RecordProperty() 是 Test 类的静态成员。因此，如果在 TEST 主体和测试装置类之外使用，则需要以 `::testing::Test::` 为前缀。
- key 必须是**有效的 XML 属性名称**，并且不能与 googletest 已经使用的名称（name、status、time、classname、type_param 和 value_param）冲突。
- 允许在测试生命周期之外调用 RecordProperty()。如果它在测试之外但在测试套件的 SetUpTestSuite() 和 TearDownTestSuite() 方法之间被调用，则它将被归因于测试套件的 XML 元素。如果它在所有测试套件之外（例如在测试环境中）被调用，它将被归因于顶级 XML 元素。

## 8. 在同一测试套件中的测试之间共享资源 ※

googletest 为每个测试创建一个新的测试夹具对象，以使测试独立且更易于调试。**然而，有时测试使用的资源设置起来很昂贵，这使得每个测试一个副本的模型成本高得令人望而却步**。

如果测试不更改资源，则共享单个资源副本没有任何害处。
因此，除了每个测试的 set-up/tear-down 之外，googletest 还支持 per-test-suite set-up/tear-down。要使用它：

1. 在您的测试夹具类（例如 FooTest ）中，**将一些成员变量声明为静态**的以保存共享资源。
2. 在您的测试装置类之外（通常就在它的下方），定义这些成员变量，**并可选择为其提供初始值**(即初始化静态变量的值)。
3. 在同一个测试夹具类中，定义一个`static void SetUpTestSuite()` 函数（记住不要把它拼写为带有小 u 的 **SetupTestSuite**！）来设置共享资源和一个`static void TearDownTestSuite()` 函数来拆除它们。

就是这样！googletest 在运行 FooTest 测试套件中的第一个测试之前（即在创建第一个 FooTest 对象之前）自动调用 SetUpTestSuite()，并在运行其中的最后一个测试之后（即在删除最后一个 FooTest 对象之后）调用 TearDownTestSuite()。
在这两者之间，测试可以使用共享资源。

请记住，**测试顺序是未定义的**，因此您的代码不能依赖于之前或之后的测试。此外，**测试不得修改任何共享资源的状态**，或者，如果它们确实修改了状态，则必须在将控制权传递给下一个测试之前将状态恢复到其原始值。

这是每个测试套件set-up和tear-down的示例：

```cpp
class FooTest : public testing::Test {
 protected:
  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  // Can be omitted if not needed.
  static void SetUpTestSuite() {
    shared_resource_ = new ...;
  }

  // Per-test-suite tear-down.
  // Called after the last test in this test suite.
  // Can be omitted if not needed.
  static void TearDownTestSuite() {
    delete shared_resource_;
    shared_resource_ = nullptr;
  }

  // You can define per-test set-up logic as usual.
  void SetUp() override { ... }

  // You can define per-test tear-down logic as usual.
  void TearDown() override { ... }

  // Some expensive resource shared by all tests.
  static T* shared_resource_;
};

T* FooTest::shared_resource_ = nullptr;

TEST_F(FooTest, Test1) {
  ... you can refer to shared_resource_ here ...
}

TEST_F(FooTest, Test2) {
  ... you can refer to shared_resource_ here ...
}
```

注意：虽然上面的代码声明 `SetUpTestSuite()` 受保护，但有时可能需要将其声明为公开的，例如与 `TEST_P` 一起使用时。

## 9. 全局Set-Up和Tear-Down ※

正如您可以在测试级别和测试套件级别进行设置和拆卸一样，您也可以在**测试程序级别**进行设置和拆卸。就是这样。

首先，您将 `::testing::Environment` 类子类化以定义一个测试环境，该环境知道如何设置和拆除：

```cpp
class Environment : public ::testing::Environment {
 public:
  ~Environment() override {}

  // Override this to define how to set up the environment.
  void SetUp() override {}

  // Override this to define how to tear down the environment.
  void TearDown() override {}
};
```

然后，您通过调用 `::testing::AddGlobalTestEnvironment()` 函数向 googletest 注册环境类的实例：

```cpp
Environment* AddGlobalTestEnvironment(Environment* env);
```

现在，当 **RUN_ALL_TESTS() 被调用时，它首先调用每个环境对象的 SetUp() 方法**，然后在没有任何环境报告致命故障并且没有调用 GTEST_SKIP() 时运行测试。 **RUN_ALL_TESTS() 总是对每个环境对象调用 TearDown()，无论测试是否运行**。

注册多个环境对象是可以的。**在这个套件中，它们的 SetUp() 将按照它们注册的顺序被调用，它们的 TearDown() 将按照相反的顺序被调用**。

请注意， **googletest 拥有已注册环境对象的所有权。因此不要自行删除它们**。

您应该在调用 RUN_ALL_TESTS() 之前调用 AddGlobalTestEnvironment()，可能在 main() 中。如果使用 gtest_main，则需要在 main() 开始之前调用它才能生效。
**一种方法是定义一个全局变量**，如下所示：

```cpp
testing::Environment* const foo_env =
    testing::AddGlobalTestEnvironment(new FooEnvironment);
```

但是，**我们强烈建议您编写自己的 main() 并在那里调用 AddGlobalTestEnvironment()**，因为依赖全局变量的初始化会使代码更难阅读，并且当您注册来自不同翻译单元的多个环境并且环境具有它们之间的依赖关系（请记住，编译器不保证来自不同翻译单元的全局变量的初始化顺序）。

## 10. 值参数化测试 ※

**值参数化测试允许您使用不同的参数测试代码，而无需编写同一测试的多个副本**。这在许多情况下很有用，例如：

- 您有一段代码，其行为受一个或多个命令行标志的影响。您希望确保您的代码针对这些标志的各种值正确执行。
- 您想要测试 OO 接口的不同实现。
- 你想在各种输入上测试你的代码（也就是数据驱动测试）。这个功能很容易被滥用，所以请在使用时发挥你的理智！

### 10.1. 如何编写值参数化测试

要编写值参数化测试，首先应该定义一个夹具(fixture)类。它必须同时从 `testing::Test` 和 `testing::WithParamInterface<T>`（后者是一个纯接口）派生，其中 T 是参数值的类型。为方便起见，您可以只从 `testing::TestWithParam<T>` 派生夹具类，它本身是从 testing::Test 和 `testing::WithParamInterface<T>` 派生的。 T 可以是任何可复制的类型。如果它是一个原始指针，你负责管理指向值的生命周期。

注意：如果您的测试装置定义了 `SetUpTestSuite()` 或 `TearDownTestSuite()`，它们必须被声明为 **public** 而不是 protected 才能使用 **TEST_P**。

```cpp
class FooTest :
    public testing::TestWithParam<const char*> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

// Or, when you want to add parameters to a pre-existing fixture class:
class BaseTest : public testing::Test {
  ...
};
class BarTest : public BaseTest,
                public testing::WithParamInterface<const char*> {
  ...
};
```

然后，使用 **TEST_P** 宏根据需要使用此夹具定义尽可能多的测试模式。`_P` 后缀用于“parameterized”或“pattern”，无论您喜欢哪个。

```cpp
TEST_P(FooTest, DoesBlah) {
  // Inside a test, access the test parameter with the GetParam() method
  // of the TestWithParam<T> class:
  EXPECT_TRUE(foo.Blah(GetParam()));
  ...
}

TEST_P(FooTest, HasBlahBlah) {
  ...
}
```

最后，您可以使用 `INSANTIATE_TEST_SUITE_P` 宏来使用您想要的任何参数集来实例化测试套件。GoogleTest 定义了许多用于生成测试参数的函数 - 请参阅测试参考中 [INSANTIATE_TEST_SUITE_P](https://google.github.io/googletest/reference/testing.html#INSTANTIATE_TEST_SUITE_P) 的详细信息。

例如，以下语句将使用 Values 参数生成器实例化来自 FooTest 测试套件的测试，每个测试都具有参数值“meeny”、“miny”和“moe”：

```cpp
INSTANTIATE_TEST_SUITE_P(MeenyMinyMoe,
                         FooTest,
                         testing::Values("meeny", "miny", "moe"));
```

**注意：上面的代码必须放在全局或命名空间范围内，而不是在函数范围内。**

INSANTIATE_TEST_SUITE_P 的第一个参数是测试套件实例化的唯一名称。下一个参数是测试模式的名称，最后一个参数是[参数生成器](https://google.github.io/googletest/reference/testing.html#param-generators)。

**您可以多次实例化一个测试模式，因此为了区分模式的不同实例，实例化名称被添加为实际测试套件名称的前缀**。请记住为不同的实例选择唯一的前缀。上面实例化的测试将具有以下名称：

- MeenyMinyMoe/FooTest.DoesBlah/0 for "meeny"
- MeenyMinyMoe/FooTest.DoesBlah/1 for "miny"
- MeenyMinyMoe/FooTest.DoesBlah/2 for "moe"
- MeenyMinyMoe/FooTest.HasBlahBlah/0 for "meeny"
- MeenyMinyMoe/FooTest.HasBlahBlah/1 for "miny"
- MeenyMinyMoe/FooTest.HasBlahBlah/2 for "moe"

您可以在 --gtest_filter 中使用这些名称。

以下语句将再次实例化来自 `FooTest` 的所有测试，每个测试都使用 [ValuesIn](https://google.github.io/googletest/reference/testing.html#param-generators) 参数生成器并具有参数值“cat”和“dog”：

```cpp
const char* pets[] = {"cat", "dog"};
INSTANTIATE_TEST_SUITE_P(Pets, FooTest, testing::ValuesIn(pets));
```

上面实例化的测试将具有以下名称：

- Pets/FooTest.DoesBlah/0 for "cat"
- Pets/FooTest.DoesBlah/1 for "dog"
- Pets/FooTest.HasBlahBlah/0 for "cat"
- Pets/FooTest.HasBlahBlah/1 for "dog"

请注意 `INSANTIATE_TEST_SUITE_P` 将实例化给定测试套件中的所有测试，无论它们的定义是在 INSANTIATE_TEST_SUITE_P 语句之前还是之后。

此外，**默认情况下，每个没有相应 INSANTIATE_TEST_SUITE_P 的 TEST_P 都会导致测试套件 GoogleTestVerification 中的测试失败**。如果您有一个测试套件，其中该遗漏不是错误，例如它位于可能因其他原因链接的库中，或者测试用例列表是动态的并且可能为空，则可以通过以下方式抑制此检查——标记测试套件：

```cpp
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(FooTest);
```

您可以查看 [sample7_unittest.cc](https://github.com/google/googletest/blob/master/googletest/samples/sample7_unittest.cc) 和 [sample8_unittest.cc](https://github.com/google/googletest/blob/master/googletest/samples/sample8_unittest.cc) 以获取更多示例。

### 参数生成器

参数 `param_generator` 是以下 GoogleTest 提供的生成测试参数的函数之一，所有这些函数都定义在 `::testing` 命名空间中：

| Parameter Generator | Behavior |
| -- | -- |
| Range(begin, end [, step]) | Yields values {begin, begin+step, begin+step+step, ...}. The values do not include end. step defaults to 1. |
| Values(v1, v2, ..., vN) | Yields values {v1, v2, ..., vN}. |
| ValuesIn(container) or ValuesIn(begin,end) | Yields values from a C-style array, an STL-style container, or an iterator range [begin, end). |
| Bool() | Yields sequence {false, true}. |
| Combine(g1, g2, ..., gN) | Yields as std::tuple n-tuples all combinations (Cartesian product) of the values generated by the given n generators g1, g2, …, gN. |

可选的最后一个参数 `name_generator` 是一个函数或函子，它根据测试参数生成自定义测试名称后缀。该函数必须接受 `TestParamInfo` 类型的参数并返回 `std::string`。测试名称后缀只能包含字母数字字符和下划线。 GoogleTest 提供了 PrintToStringParamName，或者可以使用自定义函数进行更多控制：

```cpp
INSTANTIATE_TEST_SUITE_P(
    MyInstantiation, MyTestSuite,
    ::testing::Values(...),
    [](const ::testing::TestParamInfo<MyTestSuite::ParamType>& info) {
      // Can use info.param here to generate the test suffix
      std::string name = ...
      return name;
    });
```

### 10.2. 创建值参数化抽象测试

在上面，我们在同一个源文件中定义并实例化了 FooTest。
**有时您可能希望在库中定义值参数化测试，并让其他人稍后实例化它们。这种模式被称为抽象测试**。作为其应用程序的一个示例，当您设计接口时，您可以编写一套标准的抽象测试（可能使用工厂函数作为测试参数），希望接口的所有实现都能通过。当有人实现接口时，他们可以实例化您的套件以免费获得所有接口一致性测试。

要定义**抽象测试**，您应该像这样组织代码：

1. 将参数化测试夹具类（例如 `FooTest`）的定义放在头文件中，比如 `foo_param_test.h`。将此视为声明您的抽象测试。
2. 将 `TEST_P` 定义放在 foo_param_test.cc 中，其中包括 foo_param_test.h。将此视为实现您的抽象测试。

定义它们后，您可以通过包含 foo_param_test.h、调用 INSANTIATE_TEST_SUITE_P() 并根据包含 foo_param_test.cc 的库目标来实例化它们。您可以多次实例化同一个抽象测试套件，可能在不同的源文件中。

### 10.3. 指定值参数化测试参数的名称

**INSANTIATE_TEST_SUITE_P() 的可选最后一个参数允许用户指定一个函数或函子，以根据测试参数生成自定义测试名称后缀**。
该函数应接受一个 `testing::TestParamInfo` 类型的参数，并返回 std::string。

`testing::PrintToStringParamName` 是一个内置的test后缀生成器，它返回 `testing::PrintToString(GetParam())` 的值。它不适用于 std::string 或 C 字符串。

> 注意：测试名称必须是非空的、唯一的，并且只能包含 ASCII 字母数字字符。特别是，**它们不应包含下划线**。

提供自定义函子可以更好地控制测试参数名称的生成，特别是对于自动转换不会生成有用参数名称的类型（例如上面演示的字符串）。以下示例针对多个参数、枚举类型和字符串说明了这一点，并演示了如何组合生成器。为简洁起见，它使用 lambda：

```cpp
enum class MyType { MY_FOO = 0, MY_BAR = 1 };

class MyTestSuite : public testing::TestWithParam<std::tuple<MyType, std::string>> {
};

INSTANTIATE_TEST_SUITE_P(
    MyGroup, MyTestSuite,
    testing::Combine(
        testing::Values(MyType::MY_FOO, MyType::MY_BAR),
        testing::Values("A", "B")),
    [](const testing::TestParamInfo<MyTestSuite::ParamType>& info) {
      std::string name = absl::StrCat(
          std::get<0>(info.param) == MyType::MY_FOO ? "Foo" : "Bar",
          std::get<1>(info.param));
      absl::c_replace_if(name, [](char c) { return !std::isalnum(c); }, '_');
      return name;
    });
```

## 11. 类型测试

假设您有同一个接口的多个实现，并希望确保所有实现都满足一些共同的要求。或者，您可能已经定义了几种应该符合相同“概念”的类型，并且您想要验证它。在这两种情况下，您都希望对不同类型重复相同的测试逻辑。

...

## 12. 类型参数化测试

类型参数化测试类似于类型测试，不同之处在于它们不需要您提前知道类型列表。相反，您可以先定义测试逻辑，然后用不同的类型列表实例化它。您甚至可以在同一个程序中多次实例化它。

...

## 13. 测试私有代码

如果您更改了软件的内部实现，只要用户无法观察到更改，您的测试就不会中断。因此，**根据黑盒测试原则，大多数时候您应该通过其公共接口测试您的代码**。

如果您仍然发现自己需要测试内部实现代码，请考虑是否有更好的设计。测试内部实现的愿望通常表明该类做得太多。考虑提取一个实现类并对其进行测试。然后在原始类中使用该实现类。

如果您绝对必须测试非公共接口代码，则可以。
有两种情况需要考虑：

- 静态函数（与静态成员函数不同！）或未命名的命名空间，以及
- 私有或受保护的类成员

...

## 14. “捕捉”失败

如果您正在 googletest 之上构建一个测试实用程序，您将需要测试您的实用程序。你会用什么框架来测试它？ googletest，当然。

挑战在于验证您的测试实用程序是否正确报告了失败。在通过抛出异常报告失败的框架中，您可以捕获异常并对其进行断言。但是 googletest 不使用异常，那么我们如何测试一段代码是否产生了预期的失败呢？

...

## 15. 以编程方式注册测试

`TEST` 宏处理绝大多数用例，但很少有需要运行时注册逻辑的地方。对于这些情况，框架提供了 `::testing::RegisterTest` 允许调用者动态注册任意测试。

这是一个高级 API，仅在 TEST 宏不满足时使用。**在可能的情况下，应首选宏**，因为它们避免了调用此函数的大部分复杂性。

...

## 16. 获取当前测试的名称

...

## 17. 通过处理测试事件扩展 googletest

googletest 提供了一个事件侦听器 API，可让您接收有关测试程序进度和测试失败的通知。您可以侦听的事件包括测试程序的开始和结束、测试套件或测试方法等。您可以使用此 API 来扩充或替换标准控制台输出、替换 XML 输出或提供完全不同的输出形式，例如 GUI 或数据库。例如，您还可以使用测试事件作为检查点来实现资源泄漏检查器。

...

## 18. 运行测试程序：高级选项 ※

googletest 测试程序是普通的可执行文件。构建后，您可以直接运行它们并**通过以下环境变量和/或命令行标志影响它们的行为**。要使标志起作用，您的程序必须在调用 RUN_ALL_TESTS() 之前调用 `::testing::InitGoogleTest()`。

**要查看受支持标志及其用法的列表，请使用 --help 标志运行您的测试程序。您还可以使用 -h、-? 或 /? 等简单标志**。

**如果一个选项同时由环境变量和标志指定，则后者优先**。

1. 抑制测试通过

默认情况下，googletest 为每个测试打印 1 行输出，指示它是通过还是失败。要仅显示测试失败，请使用 --gtest_brief=1 运行测试程序，或将 GTEST_BRIEF 环境变量设置为 1。

2. 抑制经过的时间

默认情况下，googletest 打印运行每个测试所需的时间。要禁用它，请使用 --gtest_print_time=0 命令行标志运行测试程序，或将 GTEST_PRINT_TIME 环境变量设置为 0。