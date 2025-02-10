cmake_minimum_required(VERSION 3.27)
project(opfs_test)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ExternalProject)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/EmbedFile.cmake)

add_subdirectory(src)
