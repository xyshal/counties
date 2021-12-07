cmake_minimum_required(VERSION 3.5)
project(test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if (UNIX)
  add_compile_options(-Wall -Wextra -Werror)
endif (UNIX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
conan_basic_setup(TARGETS)

find_package(GTest REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PROJECT_SOURCES
  main.cpp
  resource-test.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} GTest::gtest GTest::gtest_main)
