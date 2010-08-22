###
# PROJECT
###

set(PROJECT_BASENAME detect)
project(rssd-research-opencl-${PROJECT_BASENAME})
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

file(GLOB_RECURSE PROJECT_CXX_HEADER_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SRC}/*.h")
file(GLOB_RECURSE PROJECT_CPP_SOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SRC}/*.cpp")
file(GLOB_RECURSE PROJECT_CL_SOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SRC}/*.cl")
set(
    PROJECT_SOURCE_FILES
    ${PROJECT_CXX_HEADER_FILES}
    ${PROJECT_CPP_SOURCE_FILES}
    ${PROJECT_CL_SOURCE_FILES}
)

###
# CFLAGS
###

add_definitions(-Wall -Wextra) # -Werror
include_directories(./core ${PROJECT_INC} ${PROJECT_SRC} ${PROJECT_DEP})

###
# LDFLAGS
###

# if(APPLE)
#     find_library(OPENCL_LIBRARY OpenCL)
#     mark_as_advanced(OPENCL_LIBRARY)
#     set(APPLE_LIBS OPENCL_LIBRARY)
# endif(APPLE)

set(CMAKE_EXE_LINKER_FLAGS
    "-framework OpenCL"
)

###
# BUILD TARGET
###

# Create output library
add_executable(
    ${PROJECT_BASENAME}
    ${PROJECT_SOURCE_FILES}
)

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
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB}
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIB}
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BIN})

###
# DEBUG
###

# message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
if(WIN32)
    message(STATUS "CMAKE_PLATFORM_TYPE: WIN32")
elseif(APPLE)
    message(STATUS "CMAKE_PLATFORM_TYPE: APPLE")
elseif(UNIX)
    message(STATUS "CMAKE_PLATFORM_TYPE: UNIX")
endif()
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

