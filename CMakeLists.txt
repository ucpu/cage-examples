cmake_minimum_required(VERSION 3.10)
project(cage-examples CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/externals/cage/cmake)
include(cage_all)
cage_default_init()
cage_directory_link("${CMAKE_CURRENT_SOURCE_DIR}/data/cage-tests" "${CMAKE_BINARY_DIR}/result/data/cage-tests")
cage_directory_link("${CMAKE_CURRENT_SOURCE_DIR}/data/scenes-maps" "${CMAKE_BINARY_DIR}/result/scenes-maps")
cage_directory_link("${CMAKE_CURRENT_SOURCE_DIR}/data/scenes-data" "${CMAKE_BINARY_DIR}/result/data/scenes")

add_subdirectory(externals/cage)
add_subdirectory(sources)

