cmake_minimum_required(VERSION 3.8)
project(multi_node_demo)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# 添加学生信息发布者
add_executable(publisher1 src/publisher1.cpp)
target_include_directories(publisher1 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(publisher1 PUBLIC c_std_99 cxx_std_17)
ament_target_dependencies(publisher1 rclcpp std_msgs)

# 添加ROS想法发布者
add_executable(publisher2 src/publisher2.cpp)
target_include_directories(publisher2 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(publisher2 PUBLIC c_std_99 cxx_std_17)
ament_target_dependencies(publisher2 rclcpp std_msgs)

# 添加综合订阅者
add_executable(subscriber src/subscriber.cpp)
target_include_directories(subscriber PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(subscriber PUBLIC c_std_99 cxx_std_17)
ament_target_dependencies(subscriber rclcpp std_msgs)

# 安装可执行文件
install(TARGETS
  publisher1
  publisher2
  subscriber
  DESTINATION lib/${PROJECT_NAME}
)

# 测试（可选）
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()