cmake_minimum_required(VERSION 3.10)

project(PnP LANGUAGES CXX)

set(CMAKE CXX STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCES
	src/SolvePnP.cpp
)

add_executable(PnP ${SOURCES})

target_link_libraries(PnP ${OpenCV_LIBS})