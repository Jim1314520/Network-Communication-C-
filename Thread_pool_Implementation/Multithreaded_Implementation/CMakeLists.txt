cmake_minimum_required(VERSION 3.10)
project(SocketProject)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 pthread 支持
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# Socket 模块源码列表
set(SOCKET_SRC
    socket/socket.cpp
    socket/client_socket.cpp
    socket/server_socket.cpp
    socket/thread_pool.cpp
)

# 编译为静态库 core
add_library(core STATIC ${SOCKET_SRC})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(core PUBLIC Threads::Threads)

# server 可执行文件
add_executable(server server.cpp)
target_link_libraries(server PRIVATE core)

# client 可执行文件
add_executable(client client.cpp)
target_link_libraries(client PRIVATE core)

# 添加测试可执行文件
add_executable(monitor test_monitor.cpp)
target_link_libraries(monitor PRIVATE core)