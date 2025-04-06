cmake_minimum_required(VERSION 3.10)
project(common_lib C)

# 设置C标准（可选）
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB COMMON_SOURCES "*.c")  

add_library(commonlib STATIC ${COMMON_SOURCES})

# 添加头文件目录（PUBLIC 表示依赖本库的目标也会继承此路径）
target_include_directories(commonlib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 编译xxhash
add_custom_target(
    xxhash_build
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/xxHash  # 进入目录执行 make
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building xxHash..."
)

add_dependencies(commonlib xxhash_build)

target_include_directories(commonlib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/xxHash
)

target_link_libraries(commonlib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/xxHash/libxxhash.a
)

# 查找并链接线程库
find_package(Threads REQUIRED)
target_link_libraries(commonlib PRIVATE Threads::Threads)

# 链接数学库
target_link_libraries(commonlib PRIVATE m)

# 设置输出目录（可选）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)