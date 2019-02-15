cmake_minimum_required(VERSION 3.13)
project(AOS_Project)

find_package(Protobuf REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(INCLUDE_DIR include)
set(SOURCE_DIR src)
set(PROTO_DIR proto)


include_directories(${Protobuf_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/rpc_messages.proto)
set(PROTO_FILES ${PROTO_HDRS} ${PROTO_SRCS})

file(GLOB SOURCES "${SOURCE_DIR}/*.cc")

add_executable(helloworld tests/helloworld.cc ${SOURCES} ${PROTO_FILES})
target_link_libraries(helloworld ${PROTOBUF_LIBRARY})
