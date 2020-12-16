########################################################
# CMakeLists.txt
# Author: Graham Riches
# Date:December 15, 2020
# 
# Details:
#    Main CMakeLists file for building the computational-geometry
#    project
########################################################

cmake_minimum_required(VERSION 3.1...3.15)

# set the main project name
project(computational-geometry)


# find dependencies
find_package(CGAL REQUIRED)


# enable language standards
enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


# Set MSVC compiler options
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")
set(MSVC_LINK_FLAGS "/DEBUG")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LINK_FLAGS}")


# add each module
add_subdirectory(source/convex-hull)
add_subdirectory(source/segment-intersections)
add_subdirectory(lib/googletest)