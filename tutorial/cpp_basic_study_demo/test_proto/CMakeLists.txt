file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")

set(PROTO_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto_generated)

file(MAKE_DIRECTORY ${PROTO_OUTPUT_DIR})

foreach (PROTO_FILE ${PROTO_FILES})

    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    message(STATUS "${PROTO_NAME}")
    add_custom_command(
            OUTPUT ${PROTO_OUTPUT_DIR}/${PROTO_NAME}.pb.h ${PROTO_OUTPUT_DIR}/${PROTO_NAME}.pb.cc
            COMMAND protoc --cpp_out=${PROTO_OUTPUT_DIR} -I ${CMAKE_CURRENT_SOURCE_DIR} ${PROTO_FILE}
            DEPENDS ${PROTO_FILE}
    )
    #    set(PROTO_SRC ${PROTO_OUTPUT_DIR}/${PROTO_NAME}.pb.cc ${PROTO_OUTPUT_DIR}/${PROTO_NAME}.pb.h)
    #    list(APPEND PROTO_SRCS ${PROTO_SRC})

endforeach ()

set(PROTO_INCLUDE_PATH ${PROTO_OUTPUT_DIR} PARENT_SCOPE)




