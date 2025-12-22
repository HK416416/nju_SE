@echo off
echo ========================================
echo   FamilyBudgetQt 构建和测试脚本
echo ========================================

REM 设置环境变量
set MSYS2_PATH=C:\msys64
set PATH=%MSYS2_PATH%\mingw64\bin;%PATH%
set PATH=C:\Qt\6.10.0\mingw_64\bin;%PATH%
set PATH=C:\Program Files\CMake\bin;%PATH%

REM 清理旧的构建
echo 清理旧的构建文件...
if exist build rmdir /s /q build

REM 创建构建目录
mkdir build
cd build

REM 配置 CMake
echo.
echo 配置 CMake...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DENABLE_COVERAGE=ON

if errorlevel 1 (
    echo CMake 配置失败！
    pause
    exit /b 1
)

REM 构建项目
echo.
echo 构建项目...
cmake --build . --config Debug

if errorlevel 1 (
    echo 构建失败！
    pause
    exit /b 1
)

REM 运行测试
echo.
echo 运行单元测试...
cd tests
.\FamilyBudgetTests.exe --gtest_output=xml:test_results.xml

if errorlevel 1 (
    echo 测试失败！
    pause
    exit /b 1
)

echo.
echo ========================================
echo   所有测试通过！
echo ========================================

REM 打开测试报告（如果有）
if exist test_results.xml (
    echo 测试报告已生成: build/tests/test_results.xml
)

REM 生成覆盖率报告
echo.
echo 生成覆盖率报告...
cd ..
if exist coverage (
    lcov --directory . --capture --output-file coverage.info
    lcov --remove coverage.info '*/usr/*' '*/tests/*' '*/googletest/*' --output-file coverage.filtered
    genhtml coverage.filtered --output-directory coverage_report
    echo 覆盖率报告已生成: build/coverage_report/index.html
    start coverage_report/index.html
)

pause