cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(tools)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -O0 -Wall -g2 -ggdb -pthread")

# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} TOOLS_SOURCE)
add_library(tools SHARED ${CMAKE_CURRENT_SOURCE_DIR}/tools.cpp)

link_libraries(tools)
add_executable(tooltest ${CMAKE_CURRENT_SOURCE_DIR}/tooltest.cpp)