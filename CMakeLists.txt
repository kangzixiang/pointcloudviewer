cmake_minimum_required(VERSION 2.8.8)
project(Point_Cloud_Viewer)

function (add_subdirectory_if_exist arg)
    message(STATUS "add_subdirectory_if_exist: ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/CMakeLists.txt")
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/CMakeLists.txt)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0})
    endif()
endfunction ()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(OPENGLDEMO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/opengldemo/src)
set(OPENGLSTUDY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/openglstudy/src)
set(OSGDEMO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/osgdemo/src)

add_subdirectory_if_exist(opengldemo/src)
add_subdirectory_if_exist(openglstudy)
add_subdirectory_if_exist(osgdemo/src)