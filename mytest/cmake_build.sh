cmake -S . -B build
cmake --build build
cd build && ctest && cd ..

# 其他命令

make test # 相当于ctest
CTEST_OUTPUT_ON_FAILURE=TRUE ctest # 只打印失败测试的详细信息，成功的测试简单打印
CTEST_OUTPUT_ON_FAILURE=TRUE make test # 同上
ctest --output-on-failure # 同上