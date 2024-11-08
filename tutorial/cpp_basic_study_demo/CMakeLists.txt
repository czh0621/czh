

aux_source_directory(test_smart_ptr test_smart_ptr)

add_executable(s_test_smart_ptr ${test_smart_ptr})

target_link_libraries(s_test_smart_ptr spdlog::spdlog)

#

#add_subdirectory(test_core)

#

aux_source_directory(test_key_word test_key_word)

add_executable(s_test_key_word ${test_key_word})

target_link_libraries(s_test_key_word spdlog::spdlog)

#
aux_source_directory(test_class test_class)

add_executable(s_test_class ${test_class})

target_link_libraries(s_test_class spdlog::spdlog)

#
find_package(Protobuf REQUIRED)

add_subdirectory(test_proto)

aux_source_directory(${PROTO_INCLUDE_PATH} PROTO_SRCS)

add_library(test_proto STATIC ${PROTO_SRCS})

add_executable(s_test_proto test_proto/test_proto.cpp)

target_link_libraries(s_test_proto PUBLIC test_proto protobuf::libprotobuf spdlog)

target_include_directories(s_test_proto PRIVATE ${PROTO_INCLUDE_PATH})


aux_source_directory(test_memory test_memory)

add_executable(s_test_memory ${test_memory})

target_link_libraries(s_test_memory spdlog::spdlog)

#
aux_source_directory(test_stl test_stl)

add_executable(s_test_stl ${test_stl})

target_link_libraries(s_test_stl spdlog::spdlog)


aux_source_directory(test_reference test_reference)
add_executable(s_test_reference ${test_reference})
target_link_libraries(s_test_reference spdlog::spdlog)


