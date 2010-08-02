###
# PROJECT
###

set(PROJECT_BASENAME core)
project(rssd_${PROJECT_BASENAME})
cmake_minimum_required(VERSION 2.8)

###
# DEFINES
###

if(WIN32)
    add_definitions(-DRSSD_PLATFORM_WINDOWS)
elseif(APPLE)
    add_definitions(-DRSSD_PLATFORM_POSIX -DRSSD_PLATFORM_OSX)
elseif(UNIX)
    add_definitions(-DRSSD_PLATFORM_POSIX -DRSSD_PLATFORM_LINUX)
endif()

###
# VARIABLES
###

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(PROJECT_DIR "${ROOT_DIR}/${PROJECT_BASENAME}")
set(PROJECT_BIN "${PROJECT_DIR}/bin")
set(PROJECT_LIB "${PROJECT_DIR}/lib/${PROJECT_BASENAME}")
set(PROJECT_SRC "${PROJECT_DIR}/src")
set(PROJECT_DEP "${PROJECT_DIR}/dep")

###
# DEPENDENCIES
###

# find_package(stdlibs REQUIRED) # Standard C libraries
# find_package(pthread REQUIRED) # Pthread library

###
# SOURCE CODE
###

file(GLOB_RECURSE PROJECT_HEADER_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SRC}/*.h")
file(GLOB_RECURSE PROJECT_SOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SRC}/*.cpp")

# message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
#
# message(STATUS "=== Header Files ===")
# foreach(filename ${PROJECT_HEADER_FILES})
#     message(STATUS "\t=> ${filename}")
# endforeach(filename)
#
# message(STATUS "=== Source Files ===")
# foreach(filename ${PROJECT_SOURCE_FILES})
#     message(STATUS "\t=> ${filename}")
# endforeach(filename)

###
# CFLAGS
###

add_definitions(-Wall -Wextra) # -Werror
include_directories(./core ${PROJECT_INC} ${PROJECT_SRC} ${PROJECT_DEP})

###
# BUILD TARGET
###

# Create output library
add_library(
    ${PROJECT_BASENAME} STATIC
    ${PROJECT_HEADER_FILES} 
    ${PROJECT_SOURCE_FILES}) # Static Library
# add_library(core SHARED ${PROJECT_CORE_SOURCES}) # Shared Library

# Set output library property: Name
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(${PROJECT_BASENAME} PROPERTIES DEBUG_POSTFIX "_d")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(${PROJECT_BASENAME} PROPERTIES RELEASE_POSTFIX "")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set_target_properties(${PROJECT_BASENAME} PROPERTIES RELEASE_POSTFIX "_i")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set_target_properties(${PROJECT_BASENAME} PROPERTIES RELEASE_POSTFIX "_m")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Set output library property: Output Directory
set_target_properties(
    ${PROJECT_BASENAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_LIB}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_LIB}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BIN}")

