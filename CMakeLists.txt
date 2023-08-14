cmake_minimum_required(VERSION 3.10)

project(droidian-fpd)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core DBus Quick REQUIRED)

set(COMMON_SOURCES src/fpdinterface.cpp)
set(COMMON_HEADERS src/fpdinterface.h)

add_executable(droidian-fpd-client ${COMMON_SOURCES} src/fpdclient.cpp)
target_link_libraries(droidian-fpd-client Qt5::Core Qt5::DBus)
target_include_directories(droidian-fpd-client PRIVATE src)

set(FPDGUI_SOURCES
    src/fpdgui.cpp
    src/fpdinterfaceqmladapter.cpp
)

set(FPDGUI_HEADERS
    src/fpdinterfaceqmladapter.h
)

add_executable(droidian-fpd-gui ${COMMON_SOURCES} ${FPDGUI_SOURCES})
target_link_libraries(droidian-fpd-gui Qt5::Core Qt5::DBus Qt5::Quick)
target_include_directories(droidian-fpd-gui PRIVATE src)

add_executable(droidian-fpd-identify ${COMMON_SOURCES} src/fpdidentify.cpp)
target_link_libraries(droidian-fpd-identify Qt5::Core Qt5::DBus)
target_include_directories(droidian-fpd-identify PRIVATE src)

set(FPDUNLOCKER_SOURCES
    src/fpdunlocker.cpp
    src/journallistener.cpp
)

set(FPDUNLOCKER_HEADERS
    src/journallistener.h
)

add_executable(droidian-fpd-unlocker ${COMMON_SOURCES} ${FPDUNLOCKER_SOURCES})
target_link_libraries(droidian-fpd-unlocker Qt5::Core Qt5::DBus -lbatman-wrappers)
target_include_directories(droidian-fpd-unlocker PRIVATE src)

add_executable(fpidentify src/fpidentify.c)
target_link_libraries(fpidentify -lbatman-wrappers)

add_library(pam_fpd SHARED pam/pam_fpd.c)
