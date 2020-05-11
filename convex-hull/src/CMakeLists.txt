set(BINARY ${CMAKE_PROJECT_NAME})
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")

find_package(CGAL REQUIRED)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

set(SOURCES ${SOURCES})

add_library(${BINARY} SHARED ${SOURCES})

target_link_libraries(${BINARY} CGAL::CGAL)
target_link_libraries(${BINARY} JsonCGAL_lib)