cmake_minimum_required(VERSION 3.10)
project(common_lib C)

# 设置C标准（可选）
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCES prefixDict.c cutBase.c)

#target_compile_options(app PRIVATE -g)

add_subdirectory(common)

add_library(cjieba STATIC ${SOURCES})

# 添加头文件目录（PUBLIC 表示依赖本库的目标也会继承此路径）
target_include_directories(cjieba PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cjieba PUBLIC
    commonlib
)

# 设置输出目录（可选）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)