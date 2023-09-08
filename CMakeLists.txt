cmake_minimum_required(VERSION 3.4.1)

project(czh)

message(STATUS "work_dir = ${PROJECT_SOURCE_DIR} build = ${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -pthread -O2")

aux_source_directory(${PROJECT_SOURCE_DIR}/src/threadpool threadpool_src)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/future future_src)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/product/lib)

add_library(${PROJECT_NAME} STATIC ${threadpool_src} ${future_src})

# 使用 include_directories 可以在源文件中include 简写头文件，不需要写路径
#include_directories(${PROJECT_SOURCE_DIR}/src/threadpool)

add_subdirectory(tests)
