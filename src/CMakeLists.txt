cmake_minimum_required(VERSION 3.25)
project(qcounties VERSION 1.1 LANGUAGES CXX)

configure_file(config.h.in config.h)

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

find_package(fast-cpp-csv-parser REQUIRED)
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

include_directories(${fast-cpp-csv-parser_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
                      Qt6::Core
                      Qt6::Xml
                      Qt6::SvgWidgets
                      Qt6::Widgets
                      pugixml::pugixml
                      )

qt_finalize_executable(${PROJECT_NAME})

# Avoid the need for QT_QPA_PLATFORM_PLUGIN_PATH being set in dev builds
if (UNIX)
  write_file(${CMAKE_BINARY_DIR}/src/bin/qt.conf "[Paths]\nPrefix = ${CONAN_QT_ROOT}/res/archdatadir")
endif (UNIX)

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
