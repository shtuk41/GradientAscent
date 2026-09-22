cmake_minimum_required(VERSION 3.25)

option(BUILD_TESTING "Build the test suite" OFF)

if(BUILD_TESTING)
    set(VCPKG_MANIFEST_FEATURES "tests" CACHE STRING "" FORCE)
else()
    set(VCPKG_MANIFEST_FEATURES "" CACHE STRING "" FORCE)
endif()

#VCPGK

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_USE_SYSTEM_BINARIES ON CACHE BOOL "" FORCE)
set(VCPKG_FORCE_SYSTEM_BINARIES ON CACHE BOOL "" FORCE)

project(GradientAscent VERSION 0.1)

set(OPENTOPOGRAPHY_API_KEY "YOUR_API_KEY_HERE" CACHE STRING "OpenTopography API Key")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common_defs.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common_defs.h"
    @ONLY
)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(soil CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(gdal REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_path(OSMIUM_INCLUDE_DIRS "osmium/version.hpp" REQUIRED)
find_path(RAPIDXML_INCLUDE_DIRS "rapidxml/rapidxml.hpp")

add_subdirectory(source)

#if(BUILD_TESTING)
#    enable_testing()
#    add_subdirectory(tests)
#endif()

#note: add it to include in source cmake later
#target_include_directories(GradientAscent PRIVATE 
#    ${OSMIUM_INCLUDE_DIRS}
#)






