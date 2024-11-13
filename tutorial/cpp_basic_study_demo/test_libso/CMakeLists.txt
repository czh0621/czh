set(SRC export_so.cpp test_libso.cpp)
add_library(test_libso SHARED ${SRC})
target_link_libraries(test_libso)

## 源文件直接编译 所用符号信息位T 存储在代码段
#add_executable(s_test_static_load_so main1.cpp ${SRC})

# 动态库隐式链接 符号为U 未定义
add_executable(s_test_static_load_so main1.cpp ${SRC})
target_link_libraries(s_test_static_load_so test_libso)

add_executable(s_test_dynamic_load_so main2.cpp)
target_link_libraries(s_test_dynamic_load_so dl)


add_subdirectory(test_so_principle)

