cmake_minimum_required(VERSION 3.28)

enable_testing()
include(GoogleTest)

add_compile_definitions(UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
set(CMAKE_CXX_STANDARD 20)

project(gamef)

file(GLOB SRC_FILES src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(app "app.cpp" ${SRC_FILES})
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/internal)
target_precompile_headers(app PRIVATE internal/pch.h)
target_link_libraries(app PUBLIC logic)

set_property(TARGET app APPEND PROPERTY WIN32_EXECUTABLE 1)

include_directories("${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")