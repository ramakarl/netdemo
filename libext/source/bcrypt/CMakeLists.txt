cmake_minimum_required(VERSION 3.2)

OPTION(BUILD_TESTS "Build test programs" OFF)
OPTION(BUILD_STATIC "Build static libs" OFF)

project(bcrypt)

set(CMAKE_CXX_STANDARD 11)

set ( BUILD_MODE "SHARED")
if (BUILD_STATIC)
  set ( BUILD_MODE "STATIC" )
endif()

# library output
add_library(bcrypt ${BUILD_MODE} src/bcrypt_class.cpp src/blowfish.cpp)

# debug and relase libs
set ( CMAKE_DEBUG_POSTFIX "d" CACHE STRING "" )
set_target_properties( ${PROJNAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(bcrypt PRIVATE src/)
target_include_directories(bcrypt PUBLIC include/)

if(BUILD_TESTS)
  add_executable(test test/main.cpp)
  target_link_libraries(test PRIVATE bcrypt)
endif()
