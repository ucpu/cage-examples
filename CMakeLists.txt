cmake_minimum_required(VERSION 3.2)
project(cage-examples CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/externals/cage/cmake)
include(cage_all)
cage_disable_source_build()
cage_build_configuration()
cage_output_directories(${CMAKE_BINARY_DIR}/result)
cage_output_data(${CMAKE_BINARY_DIR}/result)
cage_directory_link("${CMAKE_CURRENT_SOURCE_DIR}/data/cage-tests" "${CMAKE_BINARY_DIR}/result/data/cage-tests")
cage_directory_link("${CMAKE_CURRENT_SOURCE_DIR}/data/scenes-maps" "${CMAKE_BINARY_DIR}/result/scenes-maps")
cage_directory_link("${CMAKE_CURRENT_SOURCE_DIR}/data/scenes-data" "${CMAKE_BINARY_DIR}/result/data/scenes")

add_subdirectory(externals/cage)

message(STATUS "----------------------------------------------------------------")
message(STATUS "scenes")
message(STATUS "----------------------------------------------------------------")

file(GLOB_RECURSE scenes-sources "sources/scenes/*")
add_executable(scenes ${scenes-sources})
target_link_libraries(scenes cage-client)
cage_ide_groups(scenes scenes)
cage_ide_working_dir_in_place(scenes)

