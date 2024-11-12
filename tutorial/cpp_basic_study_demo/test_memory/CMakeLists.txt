aux_source_directory(. SRC)

add_executable(s_test_memory ${SRC})

target_link_libraries(s_test_memory spdlog)