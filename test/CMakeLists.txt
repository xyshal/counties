cmake_minimum_required(VERSION 3.25)
project(test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
  add_compile_options(-Wall -Wextra -Werror)
endif (UNIX)

find_package(Qt6
             COMPONENTS
               Core
               Widgets
               SvgWidgets
               Xml
             REQUIRED)

set(CMAKE_AUTORCC ON)

find_package(fast-cpp-csv-parser REQUIRED)
find_package(GTest REQUIRED)
find_package(pugixml REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_SOURCE_DIR}/../src)
include_directories(${fast-cpp-csv-parser_INCLUDE_DIR})

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
target_link_libraries(${PROJECT_NAME} PUBLIC
                      Qt6::Core
                      Qt6::Xml
                      Qt6::SvgWidgets
                      Qt6::Widgets
                      GTest::gtest
                      GTest::gtest_main
                      pugixml::pugixml)

qt_finalize_executable(${PROJECT_NAME})

# copydlls.bat
if (WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${qt_PACKAGE_FOLDER_RELEASE}/bin/Qt6Core.dll"
    "${qt_PACKAGE_FOLDER_RELEASE}/bin/Qt6Gui.dll"
    "${qt_PACKAGE_FOLDER_RELEASE}/bin/Qt6Svg.dll"
    "${qt_PACKAGE_FOLDER_RELEASE}/bin/Qt6SvgWidgets.dll"
    "${qt_PACKAGE_FOLDER_RELEASE}/bin/Qt6Widgets.dll"
    "${qt_PACKAGE_FOLDER_RELEASE}/plugins/platforms"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${qt_PACKAGE_FOLDER_RELEASE}/plugins/platforms/qwindows.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms)

endif (WIN32)
