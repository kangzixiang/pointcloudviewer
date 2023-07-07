cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(CylinderExample)

find_package(VTK COMPONENTS 
  CommonColor
  CommonCore
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "CylinderExample: Unable to find the VTK build folder.")
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
message("---VTK_DEMO_DIR:" ${VTK_DEMO_DIR})
aux_source_directory(${VTK_DEMO_DIR} VTK_DEMO_CPP_SOURCE)
message("---CPP_SOURCE:" ${VTK_DEMO_CPP_SOURCE})
# add_executable(CylinderExample MACOSX_BUNDLE main.cpp )
add_executable(CylinderExample MACOSX_BUNDLE ${VTK_DEMO_CPP_SOURCE} )
target_link_libraries(CylinderExample PRIVATE ${VTK_LIBRARIES})
# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS CylinderExample
  MODULES ${VTK_LIBRARIES}
)
