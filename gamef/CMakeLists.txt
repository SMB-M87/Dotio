cmake_minimum_required(VERSION 3.28)

enable_testing()
include(GoogleTest)

add_compile_definitions(UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
set(CMAKE_CXX_STANDARD 20)

project(gamef)

file(GLOB SRC_FILES src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(gamef STATIC ${SRC_FILES})
target_include_directories(gamef PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(gamef PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/internal)
target_precompile_headers(gamef PRIVATE internal/pch.h)
target_link_libraries(gamef PUBLIC gmath)

include_directories("${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")