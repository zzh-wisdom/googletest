#include <gtest/gtest.h>
#include <gmock/gmock.h>

/**
 * ## 断言
 * 
 * <https://google.github.io/googletest/reference/assertions.html>
 * 
 * - ASSERT_* 致命错误
 * - EXPECT_* 非致命
 * 
 * 打印故障消息： EXPECT_EQ(x[i], y[i]) << "Vectors x and y differ at index " << i;
 * 
 * ## 使用前提
 * 
 * - #include <gtest/gtest.h>
 * 
 */
TEST(Assertions, AssertionsBase) {
  // 直接失败或成功，用于流控制
  SUCCEED(); // 纯粹用于文档记录，没有实际作用
  // FAIL() << "We shouldn't get here."; // 只能用于返回 void 的函数

  // EXPECT_THAT(value,matcher)
  // ASSERT_THAT(value,matcher)
  // Writing New Matchers Quickly: <https://google.github.io/googletest/gmock_cook_book.html#NewMatchers>
  // Matchers Reference: <https://google.github.io/googletest/reference/matchers.html>
  using ::testing::AllOf;
  using ::testing::Gt;
  using ::testing::Lt;
  using ::testing::MatchesRegex;
  using ::testing::StartsWith;
  EXPECT_THAT("Hellocsc", StartsWith("Hello"));
  // EXPECT_THAT("Line 77", MatchesRegex("Line \\d+"));
  ASSERT_THAT(6, AllOf(Gt(5), Lt(10)));

  // 单目
  bool my_condition = 1<2 && 2<1;
  EXPECT_TRUE(!my_condition) << "My condition is not true";
  EXPECT_FALSE(my_condition);

  // 双目
  EXPECT_EQ(7 * 6, 42);
  EXPECT_NE(7 * 7, 42);
  // val1<val2
  EXPECT_LT(6, 7);
  // val1<=val2
  EXPECT_LE(7, 7);
  // val1>val2
  // EXPECT_GT(val1,val2)
  // ASSERT_GT(val1,val2)
  // val1>=val2
  // EXPECT_GE(val1,val2)
  // ASSERT_GE(val1,val2)

  // 字符串
  EXPECT_STREQ("hello", "hello");
  EXPECT_STRNE("hello", "world");
  // 验证两个 C 字符串 str1 和 str2 具有相同的内容，忽略大小写。
  EXPECT_STRCASEEQ("aAa","AaA");
  ASSERT_STRCASEEQ("aAa","aAa");
  // 验证两个 C 字符串 str1 和 str2 具有不同的内容，忽略大小写。
  EXPECT_STRCASENE("a","b");
  ASSERT_STRCASENE("a","b");

  // 浮点数比较
  // 验证两个浮点值 val1 和 val2 是否大致相等，彼此相差在 4 个 ULP 以内。
  EXPECT_FLOAT_EQ(3.14, 3.14);
  // 验证两个双精度值 val1 和 val2 是否大致相等，彼此相差在 4 个 ULP 以内。
  EXPECT_DOUBLE_EQ(3.14, 3.14);
  // 验证 val1 和 val2 之间的差异是否不超过绝对误差界限 abs_error。
  ASSERT_NEAR(3.14159, 3.14158, 0.00002);
  // ASSERT_NEAR(3.14159, 3.14158, 0.00001); // 验证不通过
  EXPECT_PRED_FORMAT2(testing::FloatLE, 3.141591, 3.14159); // success, 超过float精度
  // EXPECT_PRED_FORMAT2(testing::DoubleLE, 3.141591, 3.14159); // fail


  // 异常相关的断言
  // EXPECT_THROW(statement,exception_type)
  // ASSERT_THROW(statement,exception_type)

  // 谓词断言
  // 与单独使用 EXPECT_TRUE 相比，以下断言可以验证更复杂的谓词，同时打印更清晰的失败消息。
  // EXPECT_PRED1(pred,val1)
  // EXPECT_PRED2(pred,val1,val2)
  // EXPECT_PRED3(pred,val1,val2,val3)
  // EXPECT_PRED4(pred,val1,val2,val3,val4)
  // EXPECT_PRED5(pred,val1,val2,val3,val4,val5)
  // 参数 pred 是一个函数或算子，它接受与相应宏接受值一样多的参数。
  // 如果给定参数的 pred 返回 true，则断言成功，否则断言失败。
  auto isPositive = [](int a){return a > 0; };
  EXPECT_PRED1(isPositive, 1);
  // EXPECT_PRED1(isPositive, 0);
  // 当断言失败时，它会打印每个参数的值。参数总是被评估一次。
  //注意重载函数：
  // EXPECT_PRED1(static_cast<bool (*)(int)>(IsPositive), 5);
  // EXPECT_PRED1(static_cast<bool (*)(double)>(IsPositive), 3.14);

  // EXPECT_PRED_FORMAT* 见下面的测试 TEST(GTestLearning, ExpectPredFormat)

  // 以下断言验证一段代码导致进程终止。有关上下文，请参阅死亡测试(https://google.github.io/googletest/advanced.html#death-tests)。
}


// Returns the smallest prime common divisor of m and n,
// or 1 when m and n are mutually prime.
int SmallestPrimeCommonDivisor(int m, int n) {
  int low = m > n ? m : n;
  for(int i = 2; i <= low; i++) {
    if(m%i == 0 && n%i == 0) {
      return i;
    }
  }
  return 1;
}

// Returns true if m and n have no common divisors except 1.
bool MutuallyPrime(int m, int n) {
  return SmallestPrimeCommonDivisor(m, n) == 1;
}

// A predicate-formatter for asserting that two integers are mutually prime.
testing::AssertionResult AssertMutuallyPrime(const char* m_expr,
                                             const char* n_expr,
                                             int m,
                                             int n) {
  if (MutuallyPrime(m, n)) return testing::AssertionSuccess();

  return testing::AssertionFailure() << m_expr << " and " << n_expr
      << " (" << m << " and " << n << ") are not mutually prime, "
      << "as they have a common divisor " << SmallestPrimeCommonDivisor(m, n);
}

TEST(Assertions, ExpectPredFormat) {
  const int a = 3;
  const int b = 4;
  const int c = 10;

  EXPECT_PRED_FORMAT2(AssertMutuallyPrime, a, b);  // Succeeds
  // EXPECT_PRED_FORMAT2(AssertMutuallyPrime, b, c);  // Fails
}

testing::AssertionResult IsEven(int n) {
  if ((n % 2) == 0)
    return testing::AssertionSuccess() << n << " is even";
  else
    return testing::AssertionFailure() << n << " is odd";
}
TEST(Assertions, AssertionResult) {
  EXPECT_FALSE(!IsEven(8));
}
