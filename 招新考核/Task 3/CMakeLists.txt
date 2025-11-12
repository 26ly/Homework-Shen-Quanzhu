cmake_minimum_required(VERSION 3.10)

project(Task3 LANGUAGES CXX)

set(CMAKE CXX STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
	src/Task3.cpp
)


add_executable(Task3 ${SOURCES})