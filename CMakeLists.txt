cmake_minimum_required(VERSION 3.5)
project(qcounties VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
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

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_SOURCES
  src/main.cpp
  src/mainwindow.cpp
  src/mainwindow.h
  src/mainwindow.ui
  src/svgwidget.h
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} resources/resources.qrc)

target_link_libraries(${PROJECT_NAME}
                      Qt6::Core
                      Qt6::Xml
                      Qt6::SvgWidgets
                      Qt6::Widgets
                      )

qt_finalize_executable(${PROJECT_NAME})

# Avoid the need for QT_QPA_PLATFORM_PLUGIN_PATH being set in dev builds
if (UNIX)
  write_file(${CMAKE_BINARY_DIR}/bin/qt.conf "[Paths]\nPrefix = ${CONAN_QT_ROOT}/res/archdatadir")
endif (UNIX)

# TODO: How to avoid needing to set FONTCONFIG_PATH=/etc/fonts?  Is that a
# thing the project should even be doing, or is that a system-level issue?
