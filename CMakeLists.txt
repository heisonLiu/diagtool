cmake_minimum_required(VERSION 3.10)
project(udsclient)
find_package(Threads REQUIRED)  # 查找 pthread 库



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 头文件目录
include_directories(include)

# 源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")
# 生成可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 生成静态库
add_library(${PROJECT_NAME}_static STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(udsclient PRIVATE Threads::Threads)
# 安装
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)

