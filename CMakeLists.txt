cmake_minimum_required(VERSION 3.1)
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# libigl
option(LIBIGL_WITH_OPENGL		"Use OpenGL"	ON)
option(LIBIGL_WITH_OPENGL_GLFW	"Use GLFW"	    ON)
option(LIBIGL_WITH_CGAL		    "Use CGAL"	    ON)

# cgal
if(LIBIGL_WITH_CGAL)
    find_package(CGAL REQUIRED COMPONENTS Core)
    set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL "CGAL's CMAKE Setup is super annoying ")
    include(${CGAL_USE_FILE})
endif()

find_package(LIBIGL REQUIRED QUIET)

# Add your project files
file(GLOB SRCFILES *.cpp)
file(GLOB HEADFILES *.h)
file(GLOB TPLTFILES *.hpp)

# add_library(myLib SHARED file.cpp)
add_executable(${PROJECT_NAME} ${SRCFILES} ${HEADFILES} ${TPLTFILES})
target_link_libraries(${PROJECT_NAME} igl::core igl::opengl_glfw)
