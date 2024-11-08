
add_library(singleton SHARED export_singleton.cpp)

#target_link_libraries(singleton spdlog::spdlog)

add_executable(s_test_core test_basic.cpp)

target_link_libraries(s_test_core spdlog::spdlog)