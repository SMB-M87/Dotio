cmake_minimum_required(VERSION 3.28)

enable_testing()
include(GoogleTest)

add_compile_definitions(_USE_MATH_DEFINES UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
set(CMAKE_CXX_STANDARD 20)

project(gmath)

file(GLOB SRC_FILES src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(gmath STATIC ${SRC_FILES})
target_include_directories(gmath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(gmath PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/internal)
target_precompile_headers(gmath PRIVATE internal/pch.h)

include_directories("${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")

find_package(GTest CONFIG REQUIRED)
add_executable(gmath_tests tests/main.cpp ${SRC_FILES})
target_precompile_headers(gmath_tests PRIVATE internal/pch.h)
target_link_libraries(gmath_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
