###
# PROJECT
###

set(PROJECT_BASENAME addvectors)
project(rssd-research-opencl-${PROJECT_BASENAME})
cmake_minimum_required(VERSION 2.8)

###
# INCLUDES
###

INCLUDE(CMakeFunctions.txt)

###
# DEFINES
###

# Platform(s)
if(WIN32)
    add_definitions(-DRSSD_PLATFORM_WINDOWS)
elseif(APPLE)
    add_definitions(-DRSSD_PLATFORM_POSIX -DRSSD_PLATFORM_OSX)
elseif(UNIX)
    add_definitions(-DRSSD_PLATFORM_POSIX -DRSSD_PLATFORM_LINUX)
endif()

# OpenCL
# add_definitions(-D__NO_STD_VECTOR -D__NO_STD_STRING -D__CL_ENABLE_EXCEPTIONS)
add_definitions(-D__CL_ENABLE_EXCEPTIONS)

###
# VARIABLES
###

# if(NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE "Debug")
# endif()

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
# FILES
###

rget_inode_list(${PROJECT_DIR} "*.h" HEADER_FILES)
rget_inode_list(${PROJECT_SRC} "*.cpp" SOURCE_FILES)
rget_inode_list(${PROJECT_SRC} "*.cl" OPENCL_FILES)

###
# CFLAGS
###

add_definitions(-Wall -Wextra) # -Werror
include_directories(
    ${RSSD_ROOT}
    ${PROJECT_INC}
    ${PROJECT_SRC}
    ${PROJECT_DEP})

###
# LDFLAGS
###

# if(APPLE)
#     find_library(OPENCL_LIBRARY OpenCL)
#     mark_as_advanced(OPENCL_LIBRARY)
#     set(APPLE_LIBS OPENCL_LIBRARY)
# endif(APPLE)

if(APPLE)
    # set(CMAKE_OSX_ARCHITECTURES $(ARCHS_STANDARD_32_BIT))
    # set(CMAKE_OSX_ARCHITECTURES i386)
    # set(CMAKE_C_FLAGS "-isysroot /Developer/SDKs/MacOSX10.5.sdkl")
    set(CMAKE_EXE_LINKER_FLAGS "-framework OpenCL")
endif(APPLE)

###
# BUILD TARGET
###

# Create output library
add_executable(
    ${PROJECT_BASENAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${OPENCL_FILES}
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

# Link required libraries
target_link_libraries(
    ${PROJECT_BASENAME})

###
# DEBUG
###

# message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
# if(WIN32)
#     message(STATUS "CMAKE_PLATFORM_TYPE: WIN32")
# elseif(APPLE)
#     message(STATUS "CMAKE_PLATFORM_TYPE: APPLE")
# elseif(UNIX)
#     message(STATUS "CMAKE_PLATFORM_TYPE: UNIX")
# endif()
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
