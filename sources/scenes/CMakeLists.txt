
message(STATUS "----------------------------------------------------------------")
message(STATUS "scenes")
message(STATUS "----------------------------------------------------------------")

add_executable(scenes "main.cpp")
target_link_libraries(scenes cage-engine)
cage_ide_category(scenes scenes)
cage_ide_working_dir_in_place(scenes)
