cmake_minimum_required(VERSION 3.10)
project(KalmanFilter)

set(CMAKE_CXX_STANDARD 11)

# 查找OpenCV
find_package(OpenCV REQUIRED)

# 包含头文件目录
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 设置源文件
file(GLOB SOURCE_FILES "src/*.cpp")

# 添加可执行文件
add_executable(KalmanFilter ${SOURCE_FILES})

# 链接OpenCV库
target_link_libraries(KalmanFilter ${OpenCV_LIBS})