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

set(PROTO_INCLUDE_PATH ${PROTO_OUTPUT_DIR} PARENT_SCOPE)




