cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(opengldemo1)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -O0 -Wall -g2 -ggdb -pthread")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src OPENGL_DEMO1_SOURCE)
add_executable (opengldemo1 ${OPENGL_DEMO1_SOURCE})
target_link_libraries (opengldemo1 GL GLU glut tools)