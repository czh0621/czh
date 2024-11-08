# 使用 include_directories 可以在源文件中include 简写头文件，不需要写路径
include_directories(${PROJECT_SOURCE_DIR}/src/core_util/thread_pool)
aux_source_directory(. test_thread_pool)
add_executable(s_test_thread_pool ${test_thread_pool})
target_link_libraries(s_test_thread_pool ${PROJECT_NAME})