cmake_minimum_required(VERSION 3.2)

project("cs5490_server") #rename

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

FIND_PACKAGE(Thrift)

ADD_CUSTOM_COMMAND(OUTPUT generated/sanity.cpp generated/cs5490_types.cpp generated/cs5490_constants.cpp
                   COMMAND ${THRIFT_COMPILER} -gen cpp -o generated/ ${CMAKE_CURRENT_SOURCE_DIR}/../thrift/cs5490.thrift 
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/generated/gen-cpp/* ${CMAKE_CURRENT_SOURCE_DIR}/generated
                   COMMAND rmdir ${CMAKE_CURRENT_SOURCE_DIR}/generated/gen-cpp
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../thrift/cs5490.thrift)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES cs5490.thrift)


add_executable(server src/main.cxx generated/sanity.cpp generated/cs5490_types.cpp generated/cs5490_constants.cpp)
include_directories(${THRIFT_INCLUDE_DIR})
target_link_libraries(server ${THRIFT_LIB})
