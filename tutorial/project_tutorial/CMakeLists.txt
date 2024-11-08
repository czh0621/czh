
add_subdirectory(test_thread_pool)

#
aux_source_directory(test_future test_future)
add_executable(s_test_future ${test_future})
target_link_libraries(s_test_future ${PROJECT_NAME})

#
aux_source_directory(test_limit test_limit)
add_executable(s_test_limit ${test_limit})
target_link_libraries(s_test_limit ${PROJECT_NAME})

#
aux_source_directory(test_template test_template)
add_executable(s_test_template ${test_template})
target_link_libraries(s_test_template ${PROJECT_NAME})



