
message(STATUS "----------------------------------------------------------------")
message(STATUS "gui")
message(STATUS "----------------------------------------------------------------")

macro(cage_tests folder)
	file(GLOB cage-tests-source-list "${folder}*.cpp")
	foreach(test ${cage-tests-source-list})
		get_filename_component(name ${test} NAME)
		string(REPLACE ".cpp" "" name ${name})
		add_executable(${name} ${test} gui.h)
		target_link_libraries(${name} cage-simple)
		cage_ide_category(${name} gui/${folder})
		cage_ide_working_dir_in_place(${name})
	endforeach()
endmacro()

cage_tests("layouts/")
cage_tests("texts/")
cage_tests("widgets/")
cage_tests("")

cage_embed_file(multilingualQuotes texts/multilingualQuotes.txt multilingualQuotesTxt)
cage_embed_file(utf8stressTest texts/utf8stressTest.txt utf8stressTestTxt)
