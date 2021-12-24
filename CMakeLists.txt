# CMakeList.txt : CMake project for chip8_2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("chip8_2")

set(SFML_STATIC_LIBRARIES TRUE CACHE BOOL "Use static bool libraries")

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	
	# Mac OS X: clang uses libc++ standard library
	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	endif()
endif()

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/;${CMAKE_MODULE_PATH}")
set (SFML_ROOT "C:/mydrive/garage/sfml_stuff/SFML_251")
find_package(SFML 2 COMPONENTS graphics window system network)

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	message(STATUS "foo include dir: ${SFML_INCLUDE_DIR}")
else()
	message(FATAL_ERROR "SFML library could not be found")
endif()

include_directories("include")

# Add source to this project's executable.
add_executable (chip8_2 "src/chip8_2.cpp")

target_link_libraries(chip8_2 ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

install(DIRECTORY Media DESTINATION ${PROJECT_BINARY_DIR}/Debug)

# TODO: Add tests and install targets if needed.

