
message(STATUS "----------------------------------------------------------------")
message(STATUS "scenes")
message(STATUS "----------------------------------------------------------------")

add_executable(scenes "game.cpp" "main.cpp")
target_link_libraries(scenes cage-simple)
cage_ide_category(scenes scenes)
cage_ide_working_dir_in_place(scenes)

add_executable(generateMaterialData "generateMaterialData.cpp")
target_link_libraries(generateMaterialData cage-core)
cage_ide_category(generateMaterialData scenes)
cage_ide_working_dir_in_place(generateMaterialData)
