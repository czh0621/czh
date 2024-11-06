cmake_minimum_required(VERSION 3.4.1)

option(BUILD_TUTORIAL "WHERE BUILD TEST" true)
option(BUILD_Optimization "WHERE BUILD USE Optimization " false)

project(czh)

#打印work_dir 和编译build_dir
message(STATUS "work_dir = ${PROJECT_SOURCE_DIR} build_dir = ${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -pthread ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -Wall -fsanitize=leak -g")
if (BUILD_Optimization)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif ()


aux_source_directory(${PROJECT_SOURCE_DIR}/src/future future_src)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/core_util/limit_filter limiter_src)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/core_util/thread_pool thread_pool_src)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/product/lib)

add_library(${PROJECT_NAME} STATIC ${thread_pool_src} ${limiter_src} ${thread_pool_src})

find_package(spdlog REQUIRED)

target_link_libraries(${PROJECT_NAME} spdlog::spdlog)

# 使用 include_directories 可以在源文件中include 简写头文件，不需要写路径
#include_directories(${PROJECT_SOURCE_DIR}/src/thread_pool)


if (BUILD_TUTORIAL)
    add_subdirectory(tutorial)
endif ()
