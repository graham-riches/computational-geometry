cmake_minimum_required(VERSION 3.1...3.15)
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")

project(convex_hull)

include_directories(src lib/json-cgal/src)

add_subdirectory(src)