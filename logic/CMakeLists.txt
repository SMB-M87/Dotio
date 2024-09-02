cmake_minimum_required(VERSION 3.28)

enable_testing()
include(GoogleTest)

add_compile_definitions(UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
set(CMAKE_CXX_STANDARD 20)

project(logic)

file(GLOB SRC_FILES src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(logic STATIC ${SRC_FILES}
        include/moving_object.h
        src/moving_object.cpp
        include/physics_object.h
        src/physics_object.cpp
        include/bolt.h)
target_include_directories(logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(logic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/internal)
target_precompile_headers(logic PRIVATE internal/pch.h)
target_link_libraries(logic PUBLIC gamef)

include_directories("${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")