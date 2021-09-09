# Googletest 入门

<https://google.github.io/googletest/primer.html>

- [1. 介绍，为什么要googletest](#1-介绍为什么要googletest)
- [2. 注意命名术语](#2-注意命名术语)
- [3. 基本概念](#3-基本概念)
- [4. 断言](#4-断言)
- [5. 简单测试](#5-简单测试)
- [6. Test Fixtures: 对多个测试使用相同的数据配置](#6-test-fixtures-对多个测试使用相同的数据配置)
- [7. 测试调用](#7-测试调用)
- [8. 编写main()函数](#8-编写main函数)
- [9. 已知限制](#9-已知限制)

## 1. 介绍，为什么要googletest

<https://google.github.io/googletest/primer.html#introduction-why-googletest>

- 支持任何种类的测试，不仅仅是单元测试

为了写一个好的测试，我们相信：

- 测试应该独立且可重复运行。Googletest可以隔离其他测试失败来进行快速的测试；
- 测试应该组织良好并反映被测试代码的结构。googletest 将相关测试分组为可以共享数据和子程序的测试套件（suites）
- 测试应该是可移植的和可重用的。Google 有很多与平台无关的代码；它的测试也应该是平台中立的
- 当测试失败时，他们应该提供尽可能多的关于问题的信息。googletest 不会在第一次测试失败时停止。相反，它只会停止当前测试并继续下一个测试
- 测试框架应该将测试编写者从繁琐的杂务中解放出来，让他们专注于测试内容。googletest 自动跟踪定义的所有测试，并且不需要用户枚举它们来运行。
- 测试应该很快。

googletest 基于流行的 xUnit 架构

## 2. 注意命名术语

> ⚠️注意：术语Test、Test Case和Test Suite 的不同定义可能会引起一些混淆，因此请注意辨别这些。

历史的原因，Test Case用来表示测试分组，当前的发行版中，改为使用Test Suite来表示。首选 API 是TestSuite。旧的 TestCase API 正在慢慢被弃用和重构（以防止误解）。

googletest使用Test术语对应于ISTQB和其他术语中的Test Case（测试用例）

## 3. 基本概念

1. 首先学会编写assertions，断言的结果可以是**成功、 非致命失败或致命失败**。如果发生致命故障，则中止当前功能。测试使用断言来验证代码的行为，如果测试崩溃或断言失败，则测试失败；
2. 一个测试套件（suite）包含一个或多个测试，您应该将测试分组到**反映测试代码结构**的测试套件中。当测试套件中的**多个测试需要共享公共对象和子例程**时，您可以将它们放入一个test fixture类中。
3. 一个测试程序可以包含多个测试套件。

我们现在将解释如何编写测试程序，从单个断言级别开始，然后构建测试和测试套件。

## 4. 断言

<https://google.github.io/googletest/primer.html#assertions>

- googletest 断言是类似于函数调用的**宏**;
- 当断言失败时，googletest 会打印断言的源文件和行号位置，以及一条失败消息;
- 可以提供自定义失败消息，该消息将附加到 googletest 的消息中;

断言通常成对出现，以测试相同的事物但对当前函数有不同影响的两个方面。

具有两种类型的断言：

1. ASSERT_*版本失败时会产生致命的失败，并中止当前功能；
2. EXPECT_*版本生成非致命故障，不会中止当前功能

> 通常EXPECT_\*是首选，因为它们允许在测试中报告多个失败。但是，ASSERT_\*如果在相关断言失败时继续没有意义，您应该使用。
>
> 注意：由于失败ASSERT_*立即从当前函数返回，可能会跳过后面的清理代码，因此可能会导致空间泄漏。根据泄漏的性质，它可能值得修复，也可能不值得修复 - **因此，如果除了断言错误之外还遇到堆检查器错误，请记住这一点**。

要提供自定义失败消息，只需使用<<运算符或一系列此类运算符将消息流式传输到宏中，请参阅以下示例：

```cpp
ASSERT_EQ(x.size(), y.size()) << "Vectors x and y are of unequal length";

for (int i = 0; i < x.size(); ++i) {
  EXPECT_EQ(x[i], y[i]) << "Vectors x and y differ at index " << i;
}
```

任何可以流式传输到`ostream`的东西都可以传输到断言宏中。

断言可以以各种方式验证代码的行为，如检查bool条件、基于关系运算符的运行结果、verify字符串值、浮点值等等。甚至还有断言使您能够通过提供自定义谓词来验证更复杂的状态。有关 GoogleTest 提供的断言的完整列表，请参阅[断言参考](https://google.github.io/googletest/reference/assertions.html)。

## 5. 简单测试

创建测试的步骤：

1. 使用`TEST()`宏来定义和命名测试函数，这些是不返回值的普通 C++ 函数。
2. 函数中可以包含任何有效的C++语句，以及用来检查值的各种 googletest 断言。
3. 测试的结果由断言决定；如果测试中的任何断言失败（致命或非致命），或者如果测试崩溃，则整个测试失败。否则，它成功。

```cpp
TEST(TestSuiteName, TestName) {
  ... test body ...
}
```

关于的TEST的参数：

- 第一个参数是测试套件的名称，第二个参数是测试套件中的测试名称
- 两个名称都必须是有效的 C++ 标识符，并且不应包含任何下划线 (_)
- 一个测试的全名由包含它的测试套件和它的个人名称组成。来自不同测试套件的测试可以具有相同的个人名称。

简单的例子：

```cpp
int Factorial(int n);  // Returns the factorial of n

// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) {
  EXPECT_EQ(Factorial(0), 1);
}

// Tests factorial of positive numbers.
TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(Factorial(1), 1);
  EXPECT_EQ(Factorial(2), 2);
  EXPECT_EQ(Factorial(3), 6);
  EXPECT_EQ(Factorial(8), 40320);
}
```

googletest 按测试套件对测试进行分组，因此逻辑相关的测试应该在同一个测试套件中；换句话说，它们的第一个参数 TEST()应该是相同的。在上面的示例中，我们有两个测试 HandlesZeroInput和HandlesPositiveInput，它们属于同一个测试套件FactorialTest。

> 在命名Test Suite和TEST时，您应该遵循与命名函数和类相同的约定。

## 6. Test Fixtures: 对多个测试使用相同的数据配置

如果您发现自己编写了两个或多个对类似数据进行操作的测试，则可以使用Test Fixtures。这允许您为多个不同的测试重用相同的对象配置。

要创建Fixtures：

1. 从 `::testing::Test` 派生一个类，在`protected:`标志中编写主体，因为我们希望从子类中访问fixtures成员；
2. 在类中，声明您计划使用的任何对象。
3. 如有必要，编写一个默认构造函数或SetUp()函数来为每个测试准备对象。一个常见的错误是拼写SetUp()为 Setup()（小写的u）——在 C++11 中，可以使用override来确保拼写正确。
4. 如有必要，编写一个析构函数或TearDown()函数来释放资源. 要了解何时应该使用构造函数/析构函数以及何时应该使用SetUp()/TearDown()，请阅读[FAQ](https://google.github.io/googletest/faq.html#CtorVsSetUp)。
5. 如果需要，为您的测试定义要共享的子程序。

要使用fixture，请使用TEST_F()代替TEST()，因为它允许您访问测试fixture中的对象和子例程：

```cpp
TEST_F(TestFixtureName, TestName) {
  ... test body ...
}
```

和TEST()一样，第一个参数是测试套件的名称，只不过TEST_F()的第一个参数必须是测试fixture类的名称。

> 不幸的是，C++ 宏系统不允许我们创建可以处理两种类型测试的单个宏。使用错误的宏会导致编译器错误。`_F`结尾的方法用于fixture。
>
> 您必须先定义一个测试fixture类，然后才能在 TEST_F() 中使用它 ，否则您会得到编译器错误“ virtual outside class declaration”。

**对于用 TEST_F() 定义的每个测试**，googletest 将在运行时创建一个新的测试 fixture，并立即通过 SetUp() 初始化它，再运行测试，然后通过调用清理TearDown()，删除测试fixture。**请注意，同一测试套件中的不同测试具有不同的测试fixture对象，并且 googletest 总是在创建下一个测试fixture之前删除前一个测试fixture对象。googletest并没有让多个测试重用相同的测试fixture对象。一个测试对fixture对象所做的任何更改都不会影响其他测试的fixture对象。**

例如，让我们为名为 Queue 的 FIFO 队列类编写测试，它具有以下接口：

```cpp
template <typename E>  // E is the element type.
class Queue {
 public:
  Queue();
  void Enqueue(const E& element);
  E* Dequeue();  // Returns NULL if the queue is empty.
  size_t size() const;
  ...
};
```

首先，定义一个fixture类。**按照惯例，你应该为Foo类的测试类取名为FooTest**。

```cpp
class QueueTest : public ::testing::Test {
 protected:
  void SetUp() override {
     q1_.Enqueue(1);
     q2_.Enqueue(2);
     q2_.Enqueue(3);
  }

  // void TearDown() override {}

  Queue<int> q0_;
  Queue<int> q1_;
  Queue<int> q2_;
};
```

在这种情况下，不需要TearDown()函数，因为我们不需要清理任何东西，Queue对象由它的析构函数释放。

现在我们将使用TEST_F()和这个fixture编写测试。

```cpp
TEST_F(QueueTest, IsEmptyInitially) {
  EXPECT_EQ(q0_.size(), 0);
}

TEST_F(QueueTest, DequeueWorks) {
  int* n = q0_.Dequeue();
  EXPECT_EQ(n, nullptr);

  n = q1_.Dequeue();
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(*n, 1);
  EXPECT_EQ(q1_.size(), 0);
  delete n;

  n = q2_.Dequeue();
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(*n, 2);
  EXPECT_EQ(q2_.size(), 1);
  delete n;
}
```

以上使用了ASSERT_\*和EXPECT_\*断言，经验法则是当您希望测试在断言失败后继续揭示更多错误时使用EXPECT_\*，而在失败后继续执行没有意义时使用ASSERT_\*。例如，Dequeue测试中的第二个断言是 ASSERT_NE(n, nullptr)，因为我们稍后需要对指针n取消对引用，这会在n is NULL时导致段错误。

当这些测试运行时，会发生以下情况：

1. googletest 构造一个QueueTest对象（我们称之为t1）。
2. t1.SetUp()初始化t1.
3. 在t1运行第一个测试 ( IsEmptyInitially) .
4. t1.TearDown() 测试完成后清理。
5. t1 被销毁了。
6. 在另一个QueueTest对象上重复上述步骤，这次运行DequeueWorks测试。

也就是fixture类不会被复用。

## 7. 测试调用

TEST()和TEST_F()隐式地向 googletest 注册他们的测试。定义测试后，您可以使用 RUN_ALL_TESTS() 运行它们，如果所有测试都成功，则返回0，否则返回1。请注意，RUN_ALL_TESTS() 将运行链接单元中的所有测试——它们可以来自不同的测试套件，甚至来自不同的源文件。

当调用 `RUN_ALL_TESTS()` 宏后：

1. 保存所有 googletest flags的状态。
2. 为第一个测试创建一个测试fixture对象。
3. 通过SetUp()初始化它.
4. 在fixture对象上运行测试。
5. 通过TearDown()清理TearDown()。
6. 删除TearDown()。
7. 恢复所有 googletest flags的状态。
8. 对下一个测试重复上述步骤，直到所有测试都运行完毕。

**如果发生致命故障，将跳过后续步骤。**

> 重要提示：您不能忽略 RUN_ALL_TESTS() 的返回值，否则您将收到编译器错误。这种设计的基本原理是自动化测试服务根据其退出代码而不是其 stdout/stderr 输出来确定测试是否通过；因此您的main()函数必须返回RUN_ALL_TESTS()的返回值.
>
> 此外，您应该只调用RUN_ALL_TESTS一次。调用多于一次与一些高级 googletest 功能（例如，线程安全死亡测试）冲突，因此不支持。

## 8. 编写main()函数

大多数用户不需要编写自己的main函数，而是通过gtest_main（而不是 gtest）链接，它们定义了合适的切入点。有关详细信息，请参阅此节的结尾。**此部分的其余部分仅应在您需要在测试运行之前执行无法在fixtures对象和测试套件框架内表达的自定义时才适用。**

如果您编写自己的main函数，则应返回RUN_ALL_TESTS()值。

你可以从这个样板开始：

```cpp
#include "this/package/foo.h"

#include "gtest/gtest.h"

namespace my {
namespace project {
namespace {

// The fixture for testing class Foo.
class FooTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  FooTest() {
     // You can do set-up work for each test here.
  }

  ~FooTest() override {
     // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
  }

  void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
  }

  // Class members declared here can be used by all tests in the test suite
  // for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(FooTest, MethodBarDoesAbc) {
  const std::string input_filepath = "this/package/testdata/myinputfile.dat";
  const std::string output_filepath = "this/package/testdata/myoutputfile.dat";
  Foo f;
  EXPECT_EQ(f.Bar(input_filepath, output_filepath), 0);
}

// Tests that Foo does Xyz.
TEST_F(FooTest, DoesXyz) {
  // Exercises the Xyz feature of Foo.
}

}  // namespace
}  // namespace project
}  // namespace my

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```

该::testing::InitGoogleTest()函数解析 googletest 命令行的标志，并删除所有已识别的标志。这允许用户通过各种标志来控制测试程序的行为，我们将在 [AdvancedGuide](https://google.github.io/googletest/advanced.html) 中介绍。您必须在调用 RUN_ALL_TESTS() 之前调用此函数，否则标志将无法正确初始化。

在 Windows 上，InitGoogleTest()也适用于宽字符串，因此它也可用于以UNICODE模式编译的程序。

但也许您认为编写所有这些main函数太费力了？我们完全同意您的看法，这就是 Google Test **提供 main() 基本实现的原因**。如果它符合您的需求，那么只需将您的测试与gtest_main库相关联，您就可以开始使用了。

> 注意：ParseGUnitFlags()不赞成使用InitGoogleTest().

## 9. 已知限制

**Google Test 被设计为线程安全的。该实现在pthreads库可用的系统上是线程安全的**。目前在其他系统（例如 Windows）上同时使用来自两个线程的 Googletest 断言是不安全的。在大多数测试中，这不是问题，因为通常断言是在主线程中完成的。如果您想提供帮助，您可以自愿在gtest-port.h为您的平台实现必要的同步原语。


