cmake_minimum_required(VERSION 3.15)
project(example)
include(ExternalProject)
set(INSTALLATION_PREFIX installation)
ExternalProject_Add(arx-git
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX:PATH=${INSTALLATION_PREFIX}
		-DCMAKE_BUILD_TYPE=Release
	GIT_REPOSITORY git@github.com:kdex/arx.git
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/arx-git
	UPDATE_DISCONNECTED true
)
ExternalProject_Get_Property(arx-git BINARY_DIR)
set(INSTALLATION_PATH "${BINARY_DIR}/${INSTALLATION_PREFIX}")
add_executable(${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} arx-git)
target_include_directories(${PROJECT_NAME} PRIVATE "${INSTALLATION_PATH}/include")
target_link_directories(${PROJECT_NAME} PRIVATE "${INSTALLATION_PATH}/lib")
target_link_libraries(${PROJECT_NAME} PRIVATE arx)
target_sources(${PROJECT_NAME} PRIVATE src/example.cc)