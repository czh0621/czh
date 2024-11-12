file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto_dir/*.proto")

set(PROTO_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto_generated)

file(MAKE_DIRECTORY ${PROTO_OUTPUT_DIR})

message(STATUS "start generate proto ${CMAKE_CURRENT_SOURCE_DIR} out_put_dir ${PROTO_OUTPUT_DIR}")

find_program(PROTOC protoc required)

# PROTO_FILE 为绝对路径
foreach (PROTO_FILE ${PROTO_FILES})
    message("${PROTO_FILE}")
    execute_process(COMMAND ${PROTOC} -I${CMAKE_CURRENT_SOURCE_DIR}/proto_dir --cpp_out=${PROTO_OUTPUT_DIR} ${PROTO_FILE})

endforeach ()

#set(PROTO_INCLUDE_PATH ${PROTO_OUTPUT_DIR} PARENT_SCOPE)

find_package(Protobuf REQUIRED)

aux_source_directory(${PROTO_OUTPUT_DIR} PROTO_SRCS)

add_library(test_proto STATIC ${PROTO_SRCS})

add_executable(s_test_proto test_proto.cpp)

target_link_libraries(s_test_proto PUBLIC test_proto protobuf::libprotobuf spdlog)

target_include_directories(s_test_proto PRIVATE ${PROTO_OUTPUT_DIR})




