if(WIN32)
    # 使用静态链接
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif()


cmake_minimum_required(VERSION 3.16)
project(FamilyBudgetConsole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源文件
set(SRC_FILES
    src/Transaction.cpp
    src/Budget.cpp
    src/LedgerManager.cpp
    src/AlertService.cpp
    src/SampleDataGenerator.cpp
    main.cpp
)

# 主程序
add_executable(${PROJECT_NAME} ${SRC_FILES})

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE include)

# 测试配置（保持之前的 GTest 配置不变）
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    
    # 使用 FetchContent 下载 GTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)
    
    # 单元测试程序
    # add_executable(${PROJECT_NAME}_Tests
    #     tests/MainTest.cpp
    #     tests/TransactionTest.cpp
    #     tests/BudgetTest.cpp
    #     src/Transaction.cpp
    #     src/Budget.cpp
    # )

    add_executable(${PROJECT_NAME}_Tests
    tests/IntegrationTest.cpp  # 集成测试
    src/Transaction.cpp
    src/Budget.cpp
    src/LedgerManager.cpp
    src/AlertService.cpp
    src/SampleDataGenerator.cpp
    )

    target_include_directories(${PROJECT_NAME}_Tests PRIVATE include)
    target_link_libraries(${PROJECT_NAME}_Tests PRIVATE gtest gtest_main)
    
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_Tests)
endif()