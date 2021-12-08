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

find_package(Qt6
             COMPONENTS
               Core
               Widgets
               SvgWidgets
               Xml
             REQUIRED)

set(CMAKE_AUTORCC ON)

find_package(GTest REQUIRED)
find_package(pugixml REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_SOURCE_DIR}/../src)

# TODO: Double-compiling test sources.  Make them into a static library if they
# get too numerous.
set(PROJECT_SOURCES
  main.cpp
  countydata-test.cpp
  ${PROJECT_SOURCE_DIR}/../src/county.h
  ${PROJECT_SOURCE_DIR}/../src/county.cpp
  ${PROJECT_SOURCE_DIR}/../src/countydata.h
  ${PROJECT_SOURCE_DIR}/../src/countydata.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_SOURCE_DIR}/../resources/resources.qrc)
target_link_libraries(${PROJECT_NAME}
                      Qt6::Core
                      Qt6::Xml
                      Qt6::SvgWidgets
                      Qt6::Widgets
                      GTest::gtest
                      GTest::gtest_main
                      pugixml::pugixml)

qt_finalize_executable(${PROJECT_NAME})
