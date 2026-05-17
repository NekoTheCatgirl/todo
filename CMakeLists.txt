cmake_minimum_required(VERSION 4.3)
project(todo
        VERSION 1.1.0
        DESCRIPTION "A stupidly simple TUI todolist made in pure C using ncurses"
        LANGUAGES C
)

set(CMAKE_C_STANDARD 23)

include(FetchContent)

add_executable(todo
               src/main.c
               src/todo.c
               src/tui.c
               src/config.c
               src/path_helper.c
)

if (NOT CMAKE_CROSSCOMPILING)
	find_package(json-c QUIET)
endif()

if (json-c_FOUND)
	message(STATUS "Found json-c")
	target_link_libraries(todo PRIVATE json-c::json-c)
else ()
	FetchContent_Declare(
			json-c
			GIT_REPOSITORY https://github.com/json-c/json-c.git
			GIT_TAG        json-c-0.18-20240915
			SYSTEM
			EXCLUDE_FROM_ALL
	)
	set(BUILD_APPS OFF CACHE BOOL "" FORCE)
	set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
	set(JSON_C_INSTALL_HEADERS OFF CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(json-c)

	if(TARGET json-c::json-c)
		target_link_libraries(todo PRIVATE json-c::json-c)
	else()
		target_link_libraries(todo PRIVATE json-c-static)
		target_include_directories(todo PRIVATE ${json-c_BINARY_DIR})
		file(MAKE_DIRECTORY ${json-c_BINARY_DIR}/json-c)
		file(COPY ${json-c_BINARY_DIR}/json.h DESTINATION ${json-c_BINARY_DIR}/json-c)
		file(GLOB jsonc_headers ${json-c_SOURCE_DIR}/*.h)
		file(COPY ${jsonc_headers} DESTINATION ${json-c_BINARY_DIR}/json-c)
	endif()
endif ()

FetchContent_Declare(
		argparse
		GIT_REPOSITORY https://github.com/cofyc/argparse.git
		GIT_TAG        v1.1.0
		OVERRIDE_FIND_PACKAGE
		SYSTEM
)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(argparse)

target_link_libraries(todo PRIVATE argparse)

if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_definitions(-DNCURSES_NOMACROS -DNCURSES_INTERNALS -DPDC_NCMOUSE -DPDC_WIDE -DNCURSES_BOOL=1)
	add_compile_options(-Wno-error=typedef-redefinition -fms-extensions -fpermissive)
	
	FetchContent_Declare(
			pdcurses
			GIT_REPOSITORY https://github.com/wmcbrine/PDCurses.git
			GIT_TAG        master
			SYSTEM
	)
	FetchContent_MakeAvailable(pdcurses)

	target_include_directories(todo PRIVATE src ${pdcurses_SOURCE_DIR})
	target_link_libraries(todo PRIVATE pdcurses)

	set(CPACK_GENERATOR "NSIS")
else()
	find_package(Curses REQUIRED)

	target_include_directories(todo PRIVATE src ${CURSES_INCLUDE_DIR})
	target_link_libraries(todo PRIVATE ${CURSES_LIBRARIES})
endif()

install(TARGETS todo RUNTIME DESTINATION bin)

# --- CPack config ---
set(CPACK_PACKAGE_NAME "Neko Todo")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "thea@nekothecatgirl.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Thea Nicoline Flink")

# .deb specific
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Thea Nicoline Flink <thea@nekothecatgirl.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libncurses6")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")

# .rpm specific
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Productivity")
set(CPACK_RPM_PACKAGE_REQUIRES "ncurses")

# NSIS specific
set(CPACK_NSIS_DISPLAY_NAME "Todo")
set(CPACK_NSIS_PACKAGE_NAME "Todo")
set(CPACK_NSIS_INSTALL_ROOT "C:\\\\Program Files")
set(CPACK_NSIS_MODIFY_PATH ON)

include(CPack)