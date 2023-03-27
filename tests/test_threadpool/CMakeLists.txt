add_executable(s_test_threadpool main.cpp)

#这里使用 target_include_directories 指定 main.cpp 的头文件路径 也可以在上层使用include_directories(绝对路径)
#target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/threadpool)

#target_link_directories(s_test_threadpool PUBLIC ${EXECUTABLE_OUTPUT_PATH})

target_link_libraries(s_test_threadpool ${PROJECT_NAME})