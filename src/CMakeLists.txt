cmake_minimum_required(VERSION 3.5)
project(qcounties VERSION 0.1 LANGUAGES CXX)

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

find_package(pugixml REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_SOURCES
  county.cpp
  county.h
  countydata.cpp
  countydata.h
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  preferences.cpp
  preferences.h
  svgwidget.h
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_SOURCE_DIR}/../resources/resources.qrc)

target_link_libraries(${PROJECT_NAME}
                      Qt6::Core
                      Qt6::Xml
                      Qt6::SvgWidgets
                      Qt6::Widgets
                      pugixml::pugixml
                      )

qt_finalize_executable(${PROJECT_NAME})

# Avoid the need for QT_QPA_PLATFORM_PLUGIN_PATH being set in dev builds
if (UNIX)
  write_file(${CMAKE_BINARY_DIR}/bin/qt.conf "[Paths]\nPrefix = ${CONAN_QT_ROOT}/res/archdatadir")
endif (UNIX)

# copydlls.bat
if (WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CONAN_QT_ROOT}/bin/Qt6Core.dll"
    "${CONAN_QT_ROOT}/bin/Qt6Gui.dll"
    "${CONAN_QT_ROOT}/bin/Qt6Svg.dll"
    "${CONAN_QT_ROOT}/bin/Qt6SvgWidgets.dll"
    "${CONAN_QT_ROOT}/bin/Qt6Widgets.dll"
    "${CONAN_QT_ROOT}/res/archdatadir/plugins/platforms"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CONAN_QT_ROOT}/res/archdatadir/plugins/platforms/qwindows.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms)

endif (WIN32)
