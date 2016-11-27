cmake_minimum_required(VERSION 2.8.9)

project("cs5490_server") #rename

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

FIND_PACKAGE(Thrift)
FIND_PACKAGE( Boost COMPONENTS thread system REQUIRED)
Find_package(SQLite3)
#FIND_PACKAGE(PQxx)

ADD_CUSTOM_COMMAND(OUTPUT generated/AuthServe.cpp generated/cs5490_types.cpp generated/cs5490_constants.cpp
                   COMMAND ${THRIFT_COMPILER} -gen cpp -o generated/ ${CMAKE_CURRENT_SOURCE_DIR}/../thrift/cs5490.thrift 
                   COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/generated/gen-cpp/* ${CMAKE_CURRENT_SOURCE_DIR}/generated
                   COMMAND rmdir ${CMAKE_CURRENT_SOURCE_DIR}/generated/gen-cpp
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../thrift/cs5490.thrift)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES cs5490.thrift)

#Can't use C++11; std::tuple clashes with libpqxx because reasons.
set(CMAKE_CXX_STANDARD 11)
add_definitions(-g)

add_executable(server src/main.cxx src/Log.cxx src/AuthDB.cxx src/AuthServeHandler.cxx generated/AuthServe.cpp generated/cs5490_types.cpp generated/cs5490_constants.cpp)
#include_directories(${THRIFT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${Pqxx_INCLUDE_DIRS})
#target_link_libraries(server ${THRIFT_LIB} ${Boost_LIBRARIES} ${Pqxx_LIBRARIES})
include_directories(${THRIFT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS}  ${SQLITE3_INCLUDE_DIR})
target_link_libraries(server ${THRIFT_LIB} ${Boost_LIBRARIES} ${SQLITE3_LIBRARIES} ) 
