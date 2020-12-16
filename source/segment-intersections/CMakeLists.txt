set(BINARY segment_intersections)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/segment_intersections.cpp)

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} CGAL::CGAL)