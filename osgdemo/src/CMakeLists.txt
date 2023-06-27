cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(osgdemo)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -O0 -Wall -g2 -ggdb -pthread")

find_package(OpenSceneGraph 2.0.0 REQUIRED osgDB osgUtil osgViewer osgGA)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} OSG_DEMO_SOURCE)
add_executable (osgdemo ${OSG_DEMO_SOURCE})
target_link_libraries (osgdemo ${OPENSCENEGRAPH_LIBRARIES})