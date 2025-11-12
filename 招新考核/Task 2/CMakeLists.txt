cmake_minimum_required(VERSION 3.10)

project(Task2 LANGUAGES CXX)

set(CMAKE CXX STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCES
	src/Task2.cpp
)

add_executable(Task2 ${SOURCES})

target_link_libraries(Task2 ${OpenCV_LIBS})