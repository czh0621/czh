
add_library(b SHARED b.cpp)

add_library(a SHARED a.cpp)
target_link_libraries(a b)

add_executable(s_test main.cpp)
target_link_libraries(s_test a b dl)
