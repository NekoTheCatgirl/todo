cmake_minimum_required(VERSION 4.3)
project(todo
        VERSION 1.0.0
        DESCRIPTION "A stupidly simple TUI todolist made in pure C using ncurses"
        LANGUAGES C
)

set(CMAKE_C_STANDARD 23)

include(FetchContent)

if(WIN32)
	FetchContent_Declare(
			pdcurses
			GIT_REPOSITORY https://github.com/wmcbrine/PDCurses.git
			GIT_TAG        master
	)
	FetchContent_MakeAvailable(pdcurses)

	add_executable(todo
	               src/main.c
	               src/todo.c
	               src/tui.c
	)

	target_include_directories(todo PRIVATE src ${pdcurses_SOURCE_DIR})
	target_link_libraries(todo PRIVATE pdcurses)

	set(CPACK_GENERATOR "NSIS")
else()
	find_package(Curses REQUIRED)

	add_executable(todo
	               src/main.c
	               src/todo.c
	               src/tui.c
	)

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