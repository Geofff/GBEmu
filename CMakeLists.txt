cmake_minimum_required(VERSION 2.8)
project(GBEmu)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_C_FLAGS "-std=c99 -g")
find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0) 
find_package(LibAgar COMPONENTS core gui)
include_directories(${GTK3_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${LibAgar_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})
set(GBEmu_VERSION_MAJOR 1)
set(GBEmu_VERSION_MINOR 0)
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(EXECUTABLE_OUTPUT_PATH "bin")

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_SOURCE_DIR}/config.h"
)


add_executable(GBEmu ${PROJECT_SOURCE_DIR}/main.c ${PROJECT_SOURCE_DIR}/cpu.c ${PROJECT_SOURCE_DIR}/memory.c ${PROJECT_SOURCE_DIR}/gpu.c ${PROJECT_SOURCE_DIR}/gui.c)
target_link_libraries(GBEmu ${SDL2_LIBRARY})
target_link_libraries(GBEmu ${LibAgar_LIBRARIES})
target_link_libraries(GBEmu "X11")
target_link_libraries(GBEmu "GL")
target_link_libraries(GBEmu "m")
target_link_libraries(GBEmu "GLU")
target_link_libraries(GBEmu ${GTK3_LIBRARIES})
target_link_libraries(GBEmu "pthread")
