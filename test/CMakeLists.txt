cmake_minimum_required(VERSION 3.10)
project(jiebaTest)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 创建测试可执行文件
add_executable(${PROJECT_NAME}Test
    commonTest.cpp
    initTest.cpp
    cutByProbability.cpp
)

# 链接gtest库
target_link_libraries(${PROJECT_NAME}Test
    PRIVATE
    gtest_main        # 提供默认main函数
    gmock_main        # 如果需要mock功能（可选）
    commonlib         # common
    cjieba            # jieba
)

# 注册到CTest（增强功能）
#include(GoogleTest)
#gtest_discover_tests(${PROJECT_NAME}_tests
#    EXTRA_ARGS --gtest_output=xml:test_results.xml  # 生成XML报告
#)