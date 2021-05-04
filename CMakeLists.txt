cmake_minimum_required(VERSION 3.16)
project(cage-examples CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/externals/cage/cmake)
include(cage_all)
cage_default_init()
cage_assets_add_data_path("${CMAKE_CURRENT_SOURCE_DIR}/data")
cage_directory_link("${CMAKE_CURRENT_SOURCE_DIR}/data/scenes-maps" "${CMAKE_BINARY_DIR}/result/scenes-maps")

add_subdirectory(externals/cage)
add_subdirectory(sources)

