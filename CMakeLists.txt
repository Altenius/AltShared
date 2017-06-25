cmake_minimum_required(VERSION 2.9)
project(AltShared)

find_package(Threads REQUIRED)

set(ENABLE_PROGRAMS OFF)
set(ENABLE_TESTING OFF)
add_subdirectory(lib/mbedtls)

add_subdirectory(lib/libevent)
add_subdirectory(lib/zlib)

include_directories(lib/libevent/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/libevent/include)
include_directories(lib/mbedtls/include)
include_directories(lib/zlib)

set(ALTSHARED_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/zlib ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/lib/libevent/include ${CMAKE_CURRENT_SOURCE_DIR}/lib/mbedtls/include ${CMAKE_CURRENT_BINARY_DIR}/lib/libevent/include)

add_subdirectory(src)