cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(qtglwidgetdemo)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -O0 -Wall -g2 -ggdb -pthread")

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED) # Qt COMPONENTS
#开启qt需要的uic，moc，rcc
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(PCAP)
link_libraries(${PCAP_LIBRARY})

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} QTGLWIDGET_DEMO_SOURCE)
add_executable (qtglwidgetdemo ${QTGLWIDGET_DEMO_SOURCE})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui GL)