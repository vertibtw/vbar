cmake_minimum_required(VERSION 3.16)
project(vbar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM4 REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(GTK4_LAYER_SHELL REQUIRED IMPORTED_TARGET gtk4-layer-shell-0)
pkg_check_modules(WIREPLUMBER REQUIRED IMPORTED_TARGET wireplumber-0.5)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(vbar ${SOURCES})
target_link_libraries(vbar PRIVATE
  PkgConfig::GTKMM4
  PkgConfig::GTK4_LAYER_SHELL
  PkgConfig::WIREPLUMBER
)

install(TARGETS vbar RUNTIME DESTINATION bin)
