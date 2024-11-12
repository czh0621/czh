add_library(plugin1 SHARED plugin1.cpp ../test_non_template_singleton.cpp)
#add_library(plugin1 SHARED plugin1.cpp)
target_link_libraries(plugin1)


add_library(plugin2 SHARED plugin2.cpp ../test_non_template_singleton.cpp)
#add_library(plugin2 SHARED plugin2.cpp)
target_link_libraries(plugin2)

add_executable(s_test_singleton_dll main.cpp)

target_link_libraries(s_test_singleton_dll dl)