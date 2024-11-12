option(LINK_SO "link so or use dll" false)


add_library(singleton SHARED export_singleton.cpp test_non_template_singleton.cpp)

if (LINK_SO)
    message("link so")
    add_executable(s_test_singleton main.cpp)
    # 显示加载dll dlopen 需要libdl.so
    target_link_libraries(s_test_singleton singleton dl)
else ()
    message("dll")
    add_executable(s_test_singleton main.cpp test_non_template_singleton.cpp)

    # 显示加载dll dlopen 需要libdl.so

    target_link_libraries(s_test_singleton dl)
endif ()

add_subdirectory(test_singleton_dll)




