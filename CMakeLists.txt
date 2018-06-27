cmake_minimum_required(VERSION 2.8.12)

project(msa-ui-qt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets WebEngineWidgets REQUIRED)

add_executable(msa-ui-qt src/main.cpp src/webloginwindow.cpp src/webloginwindow.h src/materialbusyindicator.cpp src/materialbusyindicator.h msaui.qrc)
target_link_libraries(msa-ui-qt Qt5::Core Qt5::Widgets Qt5::WebEngineWidgets simpleipc)