
message(STATUS "----------------------------------------------------------------")
message(STATUS "engine")
message(STATUS "----------------------------------------------------------------")

file(GLOB cage-tests-source-list "*.cpp")
foreach(test ${cage-tests-source-list})
	get_filename_component(name ${test} NAME)
	string(REPLACE ".cpp" "" name ${name})
	add_executable(${name} ${test})
	target_link_libraries(${name} cage-engine)
	cage_ide_category(${name} engine)
	cage_ide_working_dir_in_place(${name})
endforeach()
