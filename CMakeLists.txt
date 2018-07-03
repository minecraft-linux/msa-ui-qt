cmake_minimum_required(VERSION 2.8.12)

project(msa-ui-qt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets WebEngineWidgets Network REQUIRED)

add_executable(msa-ui-qt src/main.cpp src/webloginwindow.cpp src/webloginwindow.h src/materialbusyindicator.cpp src/materialbusyindicator.h src/loginipcservice.cpp src/loginipcservice.h src/loginuihandler.cpp src/loginuihandler.h src/pickaccountwindow.cpp src/pickaccountwindow.h src/msadaemonmanager.cpp src/msadaemonmanager.h src/msadaemonmanager.cpp src/profilepicturemanager.cpp src/profilepicturemanager.h msaui.qrc)
target_link_libraries(msa-ui-qt Qt5::Core Qt5::Widgets Qt5::WebEngineWidgets Qt5::Network daemon-server-utils msa-daemon-client)

install(TARGETS msa-ui-qt RUNTIME DESTINATION bin)