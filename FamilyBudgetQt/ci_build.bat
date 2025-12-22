@echo off
REM ci_build.bat - Windows下的CI构建脚本

echo ==============================
echo 开始构建家庭记账本系统
echo ==============================

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置CMake
echo 配置CMake...
cmake .. ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DBUILD_TESTS=ON

REM 编译
echo 编译项目...
cmake --build . --config Debug --parallel

REM 运行单元测试
echo 运行单元测试...
if exist "tests\Debug\FamilyBudgetQt_Tests.exe" (
  cd tests\Debug
  FamilyBudgetQt_Tests.exe
  cd ..\..
) else if exist "FamilyBudgetQt_Tests.exe" (
  FamilyBudgetQt_Tests.exe
) else (
  ctest -C Debug --output-on-failure
)

echo ==============================
echo 构建完成
echo ==============================
pause