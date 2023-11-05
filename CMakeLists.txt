cmake_minimum_required(VERSION 3.10)

set(CMAKE_INSTALL_PREFIX "/usr")

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

add_executable(fpdlistener ${COMMON_SOURCES} src/fpdlistener.cpp)
target_link_libraries(fpdlistener Qt5::Core Qt5::DBus -lbatman-wrappers)
target_include_directories(fpdlistener PRIVATE src)

add_executable(droidian-fpd-identify ${COMMON_SOURCES} src/fpdidentify.cpp)
target_link_libraries(droidian-fpd-identify Qt5::Core Qt5::DBus -lbatman-wrappers)
target_include_directories(droidian-fpd-identify PRIVATE src)

add_library(pam_fpd SHARED pam/pam_fpd.c)
set_target_properties(pam_fpd PROPERTIES OUTPUT_NAME pam_fpd)

set(ARCH_TRIPLET "")
execute_process(
    COMMAND bash -c "awk 'BEGIN { FS = \"[ ()-]\"; \"bash --version\" | getline; OFS=\"-\"; if ( /bash/ ) print $9,$11,$12 }'"
    OUTPUT_VARIABLE ARCH_TRIPLET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

install(TARGETS droidian-fpd-client droidian-fpd-gui fpdlistener
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

install(TARGETS droidian-fpd-identify
        RUNTIME DESTINATION libexec/fpd
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

install(TARGETS pam_fpd
        LIBRARY DESTINATION lib/${ARCH_TRIPLET}/security
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

install(FILES fpdlistener.service DESTINATION lib/systemd/user
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

install(FILES droidian-fpd-gui.desktop DESTINATION share/applications
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

install(FILES droidian-fingerprint.png DESTINATION share/icons
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

install(DIRECTORY qml/ DESTINATION lib/droidian-fingerprint
        FILES_MATCHING PATTERN "*.qml"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
