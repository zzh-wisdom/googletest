# include<string>
# include<gtest/gtest.h>
# include<iostream>
using namespace std;

class FixtureTest : public ::testing::Test {
 public:
  FixtureTest() {
    str_ = "Constructor";
    num_ = 1;
  }
 protected:
  void SetUp() override {
    str_ += " SetUp";
    num_++;
  }

  // void TearDown() override {}

  string str_;
  int num_;
};

TEST_F(FixtureTest, Init1) {
  cout << "Init1:" << this << endl; // Init1:0x560a48c7ef10
  str_ += " Init1";
  num_++;
  EXPECT_STREQ(str_.c_str(), "Constructor SetUp Init1");
  EXPECT_EQ(num_, 3);
}

TEST_F(FixtureTest, Init2) {
  cout << "Init2:" << this << endl; // Init2:0x55781c9a8f10 地址不一样
  str_ += " Init2";
  num_++;
  EXPECT_STREQ(str_.c_str(), "Constructor SetUp Init2");
  EXPECT_EQ(num_, 3);
}

// class TestPSuite : public testing::TestWithParam<int> {};

// TEST_P(TestPSuite, MyTest)
// {
//   std::cout << "Example Test Param: " << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_SUITE_P(MyGroup, TestPSuite, testing::Range(0, 10),
//                          testing::PrintToStringParamName());

enum class MyType { MY_FOO = 0, MY_BAR = 1 };

class MyTestSuite : public testing::TestWithParam<std::tuple<MyType, std::string>> {
};

TEST_P(MyTestSuite, MyTest)
{
  // std::cout << "Example Test Param: " << GetParam() << std::endl;
}

INSTANTIATE_TEST_SUITE_P(
    MyGroup, MyTestSuite,
    testing::Combine(
        testing::Values(MyType::MY_FOO, MyType::MY_BAR),
        testing::Values("A", "B")),
    [](const testing::TestParamInfo<MyTestSuite::ParamType>& info) {
      std::string name = (std::get<0>(info.param) == MyType::MY_FOO ? "Foo" : "Bar") + std::get<1>(info.param);
      // absl::c_replace_if(name, [](char c) { return !std::isalnum(c); }, '_');
      return name;
    });

