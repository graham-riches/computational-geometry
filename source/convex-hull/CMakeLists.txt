set(BINARY convex_hull)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/convex_hull.cpp)

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} CGAL::CGAL)