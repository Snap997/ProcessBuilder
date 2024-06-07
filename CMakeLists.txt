cmake_minimum_required(VERSION 3.27.6)
project(so VERSION 0.1.1 LANGUAGES C)

enable_testing()

set(ProcessBuilder_SOURCES src/library.c)
add_library(ProcessBuilder STATIC ${ProcessBuilder_SOURCES})

target_compile_options(ProcessBuilder PRIVATE -Wall -Wextra)
target_include_directories(ProcessBuilder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)