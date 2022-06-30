
message(STATUS "----------------------------------------------------------------")
message(STATUS "websocks")
message(STATUS "----------------------------------------------------------------")

add_executable(websock "server.cpp" "client.htm")
target_link_libraries(websock cage-core)
cage_ide_working_dir_in_place(websock)
