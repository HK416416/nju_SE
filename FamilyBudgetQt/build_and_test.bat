@echo off
echo ==========================================
echo   家庭记账本系统 - 构建与测试脚本
echo ==========================================

REM 检查是否在正确的目录
if not exist "CMakeLists.txt" (
    echo 错误：请在项目根目录运行此脚本
    pause
    exit /b 1
)

REM 清理旧的构建
if exist "build" (
    echo 清理旧的构建文件...
    rmdir /s /q build
)

REM 创建构建目录
mkdir build
cd build

REM 配置CMake
echo.
echo 配置CMake...
cmake .. -DBUILD_TESTS=ON
if %ERRORLEVEL% neq 0 (
    echo CMake配置失败
    pause
    exit /b 1
)

REM 编译项目
echo.
echo 编译项目...
cmake --build . --config Debug
if %ERRORLEVEL% neq 0 (
    echo 编译失败
    pause
    exit /b 1
)

REM 运行单元测试
echo.
echo 运行单元测试...
if exist "tests\Debug\FamilyBudgetConsole_UnitTests.exe" (
    cd tests\Debug
    FamilyBudgetConsole_UnitTests.exe
    if %ERRORLEVEL% neq 0 (
        echo 单元测试失败
        pause
        exit /b 1
    )
    cd ..\..
) else (
    echo 警告：未找到单元测试可执行文件
)

REM 运行集成测试
echo.
echo 运行集成测试...
if exist "tests\Debug\FamilyBudgetConsole_IntegrationTests.exe" (
    cd tests\Debug
    FamilyBudgetConsole_IntegrationTests.exe
    if %ERRORLEVEL% neq 0 (
        echo 集成测试失败
        pause
        exit /b 1
    )
    cd ..\..
) else (
    echo 警告：未找到集成测试可执行文件
)

REM 运行主程序（演示）
echo.
echo 运行主程序演示...
if exist "bin\Debug\FamilyBudgetConsole.exe" (
    echo 输入 "help" 查看可用命令
    echo 输入 "exit" 退出程序
    echo.
    bin\Debug\FamilyBudgetConsole.exe
) else (
    echo 警告：未找到主程序可执行文件
)

echo.
echo ==========================================
echo   构建与测试完成
echo ==========================================
pause