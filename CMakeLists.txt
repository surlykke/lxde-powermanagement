set(PROJECT lz-powermanagement)
cmake_minimum_required( VERSION 2.6 )
project(${PROJECT})

if (NOT BUILD_TOOLS_DIR)
    set (BUILD_TOOLS_DIR ${CMAKE_SOURCE_DIR}/../buildtools)
endif (NOT BUILD_TOOLS_DIR) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${BUILD_TOOLS_DIR})

find_package(Razor REQUIRED)
find_package(QtXdg REQUIRED)

set(QT_USE_QTDBUS 1)

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})

add_subdirectory(config)
add_subdirectory(battery)
add_subdirectory(idleness)
add_subdirectory(lid)
