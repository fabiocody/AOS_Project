cmake_minimum_required(VERSION 3.9)
project(AOS_Project)

find_package(Protobuf REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(INCLUDE_DIR include)
set(SOURCE_DIR src)
set(PROTO_DIR proto)
set(TESTS_DIR tests)
# set(BARBEQUE_INCLUDE "../BOSP/barbeque/include")

if (DEBUG)
    set(CMAKE_CXX_FLAGS "-DDEBUG")
endif()

include_directories(${Protobuf_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${BARBEQUE_INCLUDE})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/rpc_messages.proto)
set(PROTO_FILES ${PROTO_HDRS} ${PROTO_SRCS})

file(GLOB SOURCES "${SOURCE_DIR}/*.cc")

add_executable(test ${TESTS_DIR}/test.cc ${SOURCES} ${PROTO_FILES})
target_link_libraries(test ${PROTOBUF_LIBRARY})

add_executable(barbeque ${TESTS_DIR}/barbeque.cc ${SOURCES} ${PROTO_FILES})
target_link_libraries(barbeque ${PROTOBUF_LIBRARY})

add_executable(application ${TESTS_DIR}/application.cc ${SOURCES} ${PROTO_FILES})
target_link_libraries(application ${PROTOBUF_LIBRARY})
