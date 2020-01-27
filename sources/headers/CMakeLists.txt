
message(STATUS "----------------------------------------------------------------")
message(STATUS "cage-headers")
message(STATUS "----------------------------------------------------------------")

file(GLOB cage-headers-list "../../externals/cage/include/*/*.h")
add_executable(cage-headers main.cpp ${cage-headers-list})
set_source_files_properties(${cage-headers-list} PROPERTIES LANGUAGE CXX)
target_link_libraries(cage-headers cage-engine)
cage_ide_working_dir_in_place(cage-headers)
