
message(STATUS "----------------------------------------------------------------")
message(STATUS "cage-headers")
message(STATUS "----------------------------------------------------------------")

file(GLOB cage-headers-list "../../externals/cage/sources/include/**/*.h")
add_executable(cage-headers main.cpp ${cage-headers-list})
set(counter 0)
foreach(item IN LISTS cage-headers-list)
	get_filename_component(cpp "${item}" NAME)
	set(cpp "${cpp}.${counter}.cpp")
	configure_file("header.cpp.in" "${cpp}" @ONLY)
	target_sources(cage-headers PRIVATE "${cpp}")
	math(EXPR counter "${counter}+1")
endforeach()
#target_link_libraries(cage-headers cage-simple)
target_link_libraries(cage-headers cage-engine) # temporarily use cage-engine instead of cage-simple
cage_ide_working_dir_in_place(cage-headers)

message(STATUS "found ${counter} headers")
if(NOT(counter GREATER 10))
	message(SEND_ERROR "found no headers")
endif()

