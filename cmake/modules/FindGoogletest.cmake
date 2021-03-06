function(setup_googletest_from_repository)
	message(STATUS "Here")
	set(path_include_gtest "${path_googletest_repository}/googletest/include")
	set(path_include_gmock "${path_googletest_repository}/googlemock/include")
	message(STATUS "Google Test repository at ${path_googletest_repository}")
	message(STATUS "path_include_gtest: ${path_include_gtest}")
	message(STATUS "path_include_gmock: ${path_include_gmock}")
	set(have_gtest TRUE CACHE INTERNAL "have_gtest")
	set(have_gtest_from_repository TRUE CACHE INTERNAL "have_gtest_from_repository")
	add_subdirectory(${path_googletest_repository} googletest)
endfunction()

set(REQUIRE_GTEST FALSE CACHE BOOL "Require Google Test")

if (REQUIRE_GTEST)
	find_library(GMOCK_MAIN_LIB gmock_main)
	if (GMOCK_MAIN_LIB)
		message(STATUS "Google Test found")
		set(have_gtest TRUE)
	elseif (path_googletest_repository)
		setup_googletest_from_repository()
	else()
		find_path(path_googletest_repository NAMES "googletest/include/gtest/gtest.h" HINTS "../ggoogletest")
		if (path_googletest_repository)
			setup_googletest_from_repository()
		endif()
	endif()
	if (NOT have_gtest)
	message(STATUS "There")
		message(FATAL_ERROR "Google Test required but not found")
	endif()
else()
	message(STATUS "Tests disabled")
endif()

function(add_gtest_to_target tgt)
	if (have_gtest_from_repository)
		add_dependencies(${tgt} gtest)
		target_compile_definitions(${tgt} PRIVATE HAVE_GTEST=1)
		target_include_directories(${tgt} PRIVATE ${path_include_gtest} ${path_include_gmock})
		target_link_libraries(${tgt} gtest_main gmock_main)
	elseif (have_gtest)
		target_compile_definitions(${tgt} PRIVATE HAVE_GTEST=1)
		target_link_libraries(${tgt} ${GMOCK_MAIN_LIB})
	endif()
endfunction()
