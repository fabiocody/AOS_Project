cmake_minimum_required(VERSION 3.13)
project(AOS_Project)

find_package(Protobuf REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(INCLUDE_DIR include)
set(SOURCE_DIR src)
set(PROTO_DIR proto)
set(TESTS_DIR tests)
set(BARBEQUE_INCLUDE "../BOSP/barbeque/include")

include_directories(${Protobuf_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${BARBEQUE_INCLUDE})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/rpc_messages.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/test_msg.proto)
set(PROTO_FILES ${PROTO_HDRS} ${PROTO_SRCS})

file(GLOB SOURCES "${SOURCE_DIR}/*.cc")

add_executable(producer ${TESTS_DIR}/producer.cc ${SOURCES} ${PROTO_FILES})
target_link_libraries(producer ${PROTOBUF_LIBRARY})

add_executable(consumer ${TESTS_DIR}/consumer.cc ${SOURCES} ${PROTO_FILES})
target_link_libraries(consumer ${PROTOBUF_LIBRARY})
