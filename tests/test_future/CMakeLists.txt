add_executable(s_test_future main.cpp)

#target_link_directories(s_test_threadpool PUBLIC ${PROJECT_SOURCE_DIR}/threadpool)

target_link_libraries(s_test_future ${PROJECT_NAME})