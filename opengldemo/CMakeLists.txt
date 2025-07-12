cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(opengl_demo)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -O0 -Wall -g2 -ggdb -pthread")

find_library(PCAP_LIBRARY pcap)
link_libraries(${PCAP_LIBRARY})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src OPENGL_DEMO_SOURCE)
add_executable (opengldemo ${OPENGL_DEMO_SOURCE})
target_link_libraries (opengldemo GL GLU glut)