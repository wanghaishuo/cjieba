cmake_minimum_required(VERSION 3.10)
project(jiebaTest C)

# 设置C标准（可选）
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 默认启用Debug构建类型（自动添加-g）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 根据构建类型定义宏
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("not -g")
    message(${CMAKE_CXX_FLAGS})
    add_compile_definitions(NDEBUG)
endif()

add_subdirectory(src/common)

set(SOURCES src/cutBase.c src/prefixDict.c src/cjieba.c)

add_library(cjieba STATIC ${SOURCES})

# 添加头文件目录（PUBLIC 表示依赖本库的目标也会继承此路径）
target_include_directories(cjieba PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cjieba PUBLIC
    commonlib
)

add_executable(jiebaTest
    main.c
)

target_link_libraries(jiebaTest PUBLIC
    cjieba
)
