cmake_minimum_required(VERSION 3.10)
project(NaiLongTracking)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(ONNXRUNTIME_ROOT "/home/metal_angel/Desktop/Code/ly/onnxruntime")

# 查找OpenCV
find_package(OpenCV REQUIRED)

# 包含目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# 添加可执行文件
add_executable(nai_long_tracking
    src/main.cpp
    src/Detector.cpp
    src/KalmanTracker.cpp
    src/MultiTracker.cpp
    src/Visualization.cpp
)

# 链接OpenCV库
target_link_libraries(nai_long_tracking ${OpenCV_LIBS})