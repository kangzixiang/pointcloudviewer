cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(qttest)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -O0 -Wall -g2 -ggdb -pthread")

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED) # Qt COMPONENTS

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} QT_TEST_SOURCE)
add_executable (${PROJECT_NAME} ${QT_TEST_SOURCE})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui GLESv2)