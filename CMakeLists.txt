cmake_minimum_required(VERSION 3.9.4)
project(arx CXX)
if(WIN32 AND NOT CYGWIN)
	set(DEF_INSTALL_CMAKE_DIR cmake)
else()
	set(DEF_INSTALL_CMAKE_DIR lib/cmake/${PROJECT_NAME})
endif()
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")
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
	src/Argument.h
	src/ArgumentContainer.cc
	src/ArgumentContainer.h
	src/ArgumentMap.h
	src/Castable.cc
	src/Castable.h
	src/format/format.cc
	src/format/format.h
	src/format/Indenter.cc
	src/format/Indenter.h
	src/Option.cc
	src/Option.h
	src/OptionContainer.cc
	src/OptionContainer.h
	src/Parser.cc
	src/Parser.h
	src/Parser.Internal.h
	src/Results.cc
	src/Results.h
	src/Results.Internal.h
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
export(PACKAGE ${PROJECT_NAME})
set(PROJECT_TARGET_FILE "${PROJECT_NAME}-targets.cmake")
set(PROJECT_CONFIG_FILE "${PROJECT_NAME}-config.cmake")
set(PROJECT_VERSION_FILE "${PROJECT_NAME}-config-version.cmake")
include(CMakePackageConfigHelpers)
configure_package_config_file(
	${PROJECT_CONFIG_FILE}.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG_FILE}"
	INSTALL_DESTINATION ${INSTALL_LIB_DIR}/${PROJECT_NAME}/cmake
	PATH_VARS INSTALL_INCLUDE_DIR
)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_VERSION_FILE}
	VERSION 1.0.0
	COMPATIBILITY SameMajorVersion
)
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CONFIG_FILE}
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_VERSION_FILE}
	DESTINATION "${INSTALL_CMAKE_DIR}"
	COMPONENT dev
)
install(
	EXPORT ${PROJECT_TARGET_FILE}
	DESTINATION "${INSTALL_CMAKE_DIR}"
	COMPONENT dev
)
install(
	TARGETS ${PROJECT_NAME}
	EXPORT "${PROJECT_TARGET_FILE}"
	RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
)
install(DIRECTORY src/
	COMPONENT dev
	DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}"
	REGEX ".cc$" EXCLUDE
	REGEX ".Internal.h$" EXCLUDE
)
export(
	TARGETS ${PROJECT_NAME}
	FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_TARGET_FILE}"
)