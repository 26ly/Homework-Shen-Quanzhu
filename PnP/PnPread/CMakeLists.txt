cmake_minimum_required(VERSION 3.10)

project(Read LANGUAGES CXX)

set(CMAKE CXX STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCES
	src/read.cpp
)

add_executable(Read ${SOURCES})

target_link_libraries(Read ${OpenCV_LIBS})