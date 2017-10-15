cmake_minimum_required(VERSION 3.9.4)
project(apx CXX)
set(${PROJECT_NAME}_MAJOR_VERSION 1)
set(${PROJECT_NAME}_MINOR_VERSION 0)
set(${PROJECT_NAME}_PATCH_VERSION 0)
set(PROJECT_VERSION
	${${PROJECT_NAME}_MAJOR_VERSION}.
	${${PROJECT_NAME}_MINOR_VERSION}.
	${${PROJECT_NAME}_PATCH_VERSION}
)
if(WIN32 AND NOT CYGWIN)
	set(DEF_INSTALL_CMAKE_DIR CMake)
else()
	set(DEF_INSTALL_CMAKE_DIR lib/CMake/FooBar)
endif()
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")
# Make relative paths absolute
foreach(p LIB BIN INCLUDE CMAKE)
	set(x INSTALL_${p}_DIR)
	if(NOT IS_ABSOLUTE "${${x}}")
		set(${x} "${CMAKE_INSTALL_PREFIX}/${${x}}")
	endif()
endforeach()
include_directories(include)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(GLOBAL_CXX_FLAGS
	"\
	-fno-builtin\
	-pedantic\
	-Wall\
	-Wcast-align\
	-Wextra\
	-Wfloat-equal\
	-Wmissing-declarations\
	-Woverloaded-virtual\
	-Wredundant-decls\
	-Wundef\
	-Wwrite-strings\
	-Wno-parentheses\
	"
)
set(GLOBAL_CXX_RELEASE_FLAGS
	"\
	${GLOBAL_CXX_FLAGS}\
	"
)
set(GLOBAL_CXX_DEBUG_FLAGS
	"\
	${GLOBAL_CXX_FLAGS}\
	-fsanitize=address\
	-fsanitize=undefined\
	"
)
set(GLOBAL_CXX_WASM_FLAGS
	"\
	-emit-llvm\
	--target=wasm64\
	-S\
	-Oz\
	"
)
option(WASM "Output a `wasm` binary" OFF)
if(WASM)
	set(GLOBAL_CXX_RELEASE_FLAGS
		"${GLOBAL_CXX_RELEASE_FLAGS}\
		${GLOBAL_CXX_WASM_FLAGS}\
		"
	)
else()
	set(GLOBAL_CXX_RELEASE_FLAGS
		"\
		${GLOBAL_CXX_RELEASE_FLAGS}\
		-O3\
		"
	)
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLOBAL_CXX_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GLOBAL_CXX_DEBUG_FLAGS}")
set(CMAKE_CXX_STANDARD 17)
add_library(${PROJECT_NAME}
	src/Argument.cc
	src/ArgumentContainer.cc
	src/Castable.cc
	src/format/format.cc
	src/format/Indenter.cc
	src/Option.cc
	src/OptionContainer.cc
	src/Parser.cc
	src/Results.cc
)
add_executable(example
	example/example.cc
)
target_link_libraries(example
	apx
)
add_definitions(-fdiagnostics-color=always)
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)
if(UNIX AND NOT APPLE)
	execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
	if("${LD_VERSION}" MATCHES "GNU gold")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
	endif()
endif()
export(TARGETS ${PROJECT_NAME}
	FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
)
export(PACKAGE ${PROJECT_NAME})
set(PROJECT_CONFIG_FILE "${PROJECT_NAME}-config.cmake")
set(PROJECT_VERSION_FILE "${PROJECT_NAME}-config-version.cmake")
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
configure_file(${PROJECT_CONFIG_FILE}.in "${PROJECT_BINARY_DIR}/${PROJECT_CONFIG_FILE}" @ONLY)
configure_file(${PROJECT_VERSION_FILE}.in "${PROJECT_BINARY_DIR}/${PROJECT_VERSION_FILE}" @ONLY)
install(FILES
	"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_CONFIG_FILE}"
	"${PROJECT_BINARY_DIR}/${PROJECT_VERSION_FILE}"
	DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT include
)
install(EXPORT ${PROJECT_NAME}Targets DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT include)
install(TARGETS ${PROJECT_NAME}
	EXPORT "${PROJECT_NAME}Targets"
	RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
	INCLUDES DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}" COMPONENT include
)