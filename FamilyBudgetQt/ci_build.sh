#!/bin/bash
# ci_build.sh - CI环境下的构建脚本

set -e  # 出现错误时退出

echo "=============================="
echo "开始构建家庭记账本系统"
echo "=============================="

# 创建构建目录
mkdir -p build
cd build

# 配置CMake
echo "配置CMake..."
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTS=ON \
  -DCMAKE_CXX_FLAGS="--coverage" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"

# 编译
echo "编译项目..."
make -j$(nproc)

# 运行单元测试
echo "运行单元测试..."
if [ -f "tests/FamilyBudgetQt_Tests" ]; then
  ./tests/FamilyBudgetQt_Tests --gtest_output="xml:test_results.xml"
else
  ctest --output-on-failure --verbose
fi

# 生成覆盖率报告
echo "生成覆盖率报告..."
if command -v lcov &> /dev/null; then
  lcov --capture --directory . --output-file coverage.info
  lcov --remove coverage.info \
    '/usr/*' \
    '*/tests/*' \
    '*/googletest/*' \
    --output-file coverage.info
  genhtml coverage.info --output-directory coverage_report
  echo "覆盖率报告已生成: build/coverage_report/index.html"
else
  echo "警告: lcov未安装，跳过覆盖率报告生成"
fi

echo "=============================="
echo "构建完成"
echo "=============================="