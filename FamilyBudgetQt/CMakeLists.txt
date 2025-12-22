cmake_minimum_required(VERSION 3.16)
project(FamilyBudgetConsole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 覆盖率编译选项（在CI中启用）
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
if(ENABLE_COVERAGE)
    message(STATUS "Enabling code coverage")
    add_compile_options(--coverage -g -O0)
    add_link_options(--coverage)
endif()

# Windows特定设置
if(WIN32)
    # 使用静态链接
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif()

# 主程序源文件
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

# ============ 测试配置 ============
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    message(STATUS "Building tests...")
    
    # 使用 FetchContent 下载 GTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(CMAKE_MACOSX_RPATH ON)
    FetchContent_MakeAvailable(googletest)
    
    # 测试源文件
    set(TEST_SRC_FILES
        tests/MainTest.cpp
        tests/TransactionTest.cpp
        tests/BudgetTest.cpp
        tests/IntegrationTest.cpp
    )
    
    # 单元测试程序 - 测试核心类
    add_executable(${PROJECT_NAME}_UnitTests
        tests/MainTest.cpp
        tests/TransactionTest.cpp
        tests/BudgetTest.cpp
        src/Transaction.cpp
        src/Budget.cpp
    )
    
    target_include_directories(${PROJECT_NAME}_UnitTests PRIVATE include)
    target_link_libraries(${PROJECT_NAME}_UnitTests 
        PRIVATE 
        gtest 
        gtest_main
    )
    
    # 集成测试程序 - 测试完整功能
    add_executable(${PROJECT_NAME}_IntegrationTests
        tests/IntegrationTest.cpp
        src/Transaction.cpp
        src/Budget.cpp
        src/LedgerManager.cpp
        src/AlertService.cpp
        src/SampleDataGenerator.cpp
        #main.cpp  # 
    
    target_include_directories(${PROJECT_NAME}_IntegrationTests PRIVATE include)
    target_link_libraries(${PROJECT_NAME}_IntegrationTests 
        PRIVATE 
        gtest 
        gtest_main
    )
    
    # 启用测试发现
    enable_testing()
    
    # 添加测试
    add_test(NAME ${PROJECT_NAME}_UnitTests COMMAND ${PROJECT_NAME}_UnitTests)
    add_test(NAME ${PROJECT_NAME}_IntegrationTests COMMAND ${PROJECT_NAME}_IntegrationTests)
    
    # 为覆盖率配置测试目标
    if(ENABLE_COVERAGE)
        target_compile_options(${PROJECT_NAME}_UnitTests PRIVATE --coverage)
        target_link_libraries(${PROJECT_NAME}_UnitTests PRIVATE --coverage)
        target_compile_options(${PROJECT_NAME}_IntegrationTests PRIVATE --coverage)
        target_link_libraries(${PROJECT_NAME}_IntegrationTests PRIVATE --coverage)
    endif()
    
    # 输出目录设置
    set_target_properties(${PROJECT_NAME}_UnitTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
    set_target_properties(${PROJECT_NAME}_IntegrationTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)