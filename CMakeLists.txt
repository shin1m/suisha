cmake_minimum_required(VERSION 3.12)
project(suisha VERSION 202408)
add_subdirectory(src)
enable_testing()
add_subdirectory(test)
add_subdirectory(examples)
install(DIRECTORY include/suisha DESTINATION include)
install(TARGETS suisha EXPORT suisha-targets DESTINATION lib)
install(EXPORT suisha-targets FILE suisha-targets.cmake DESTINATION lib/cmake/suisha-${suisha_VERSION})
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/suisha-config.cmake"
	INSTALL_DESTINATION "lib/cmake/suisha-${suisha_VERSION}"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
	)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/suisha-config-version.cmake"
	VERSION "${suisha_VERSION_MAJOR}.${suisha_VERSION_MINOR}"
	COMPATIBILITY AnyNewerVersion
	)
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/suisha-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/suisha-config-version.cmake"
	DESTINATION "lib/cmake/suisha-${suisha_VERSION}"
	)
