cmake_minimum_required(VERSION 3.10)
project(SocketProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件路径
include_directories(${CMAKE_SOURCE_DIR}/socket)

# 收集 socket 源文件
set(SOCKET_SRC
    socket/socket.cpp
    socket/client_socket.cpp
    socket/server_socket.cpp
    socket/thread_pool.cpp
)

# 添加可执行文件 server
add_executable(server server.cpp ${SOCKET_SRC})

# 添加可执行文件 client
add_executable(client client.cpp ${SOCKET_SRC})

#  关键补充：链接 pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(server Threads::Threads)
target_link_libraries(client Threads::Threads)
