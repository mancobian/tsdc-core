###
# PROJECT
###

set(PROJECT_BASENAME core)
project(rssd-${PROJECT_BASENAME})
cmake_minimum_required(VERSION 2.8)

###
# INCLUDES
###

INCLUDE(CMakeFunctions.txt)

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

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

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

get_inode_list(${PROJECT_DIR} "*" PROJECT_TOPLEVEL_INODES)
get_inode_list(${PROJECT_SRC} "*" PROJECT_SOURCE_TOPLEVEL_INODES)
rget_inode_list(${PROJECT_DIR} "*.h" PROJECT_HEADER_FILES)
rget_inode_list(${PROJECT_SRC} "*.cpp" PROJECT_SOURCE_FILES)

string(COMPARE NOTEQUAL ${CMAKE_BUILD_TYPE} "Test" IS_BUILD_TYPE_TEST)
if(${IS_BUILD_TYPE_TEST})
    remove_inodes_from_list(${PROJECT_SRC} "${PROJECT_SOURCE_FILES}" "test" RESULT_FILES)
else()
    remove_inodes_from_list(${PROJECT_SRC} "${PROJECT_SOURCE_FILES}" "dummy" RESULT_FILES)
endif()
set(PROJECT_SOURCE_FILES ${RESULT_FILES})

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

# Set output library property: Name
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set_target_properties(${PROJECT_BASENAME} PROPERTIES DEBUG_POSTFIX "_d")
# elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#     set_target_properties(${PROJECT_BASENAME} PROPERTIES RELEASE_POSTFIX "")
# elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
#     set_target_properties(${PROJECT_BASENAME} PROPERTIES RELEASE_POSTFIX "_i")
# elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
#     set_target_properties(${PROJECT_BASENAME} PROPERTIES RELEASE_POSTFIX "_m")
# endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Set output library property: Output Directory
set_target_properties(
    ${PROJECT_BASENAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_LIB}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_LIB}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BIN}")

###
# INSTALL
###

# Local install vars
set(HEADER_FILE_PATTERNS "*.h")
set(RSSD_HEADER_INSTALL_DIRECTORY "include/rssd")
set(PROJECT_HEADER_INSTALL_DIRECTORY "${RSSD_HEADER_INSTALL_DIRECTORY}/core")
set(META_HEADER_FILE "${PROJECT_DIR}/${PROJECT_BASENAME}.h")
set(EXCLUDE_HEADER_DIRS dummy test)
set(TOP_LEVEL_DIRS ${PROJECT_TOP_LEVEL_DIR} ${PROJECT_SOURCE_TOP_LEVEL_DIR})

# Install project header directory(s)
get_dir_list_by_exclude(${PROJECT_SRC} "${EXCLUDE_HEADER_DIRS}" RESULTS)
foreach(DIR ${RESULTS})
     install(
        DIRECTORY ${DIR}
        DESTINATION ${PROJECT_HEADER_INSTALL_DIRECTORY}
        FILES_MATCHING PATTERN "*.h"
        PATTERN "*.cpp" EXCLUDE)
endforeach()

# Install project top-level header file(s)
get_inode_list(${PROJECT_SRC} "*.h" RESULTS)
foreach(FILE ${RESULTS})
    install(
        FILES ${FILE}
        DESTINATION ${PROJECT_HEADER_INSTALL_DIRECTORY})
endforeach()

# Install project meta-header file
get_inode_list(${PROJECT_DIR} "*.h" RESULTS)
foreach(FILE ${RESULTS})
    install(
        FILES ${FILE}
        DESTINATION ${RSSD_HEADER_INSTALL_DIRECTORY})
endforeach()

# Install project targets
install(TARGETS ${PROJECT_BASENAME}
  ARCHIVE DESTINATION lib)

###
# DEBUG
###

# message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
#
# message(STATUS "=== Header Files ===")
# foreach(filename ${PROJECT_HEADER_FILES})
#     message(STATUS "${filename}")
# endforeach(filename)
#
# message(STATUS "=== Source Files ===")
# foreach(filename ${PROJECT_SOURCE_FILES})
#     message(STATUS "\t=> ${filename}")
# endforeach(filename)
