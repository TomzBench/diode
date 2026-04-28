cmake_minimum_required(VERSION 3.20)
project(diode_simple_e2e C)

execute_process(
    COMMAND diode cmake-dir
    OUTPUT_VARIABLE DIODE_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)
find_package(Diode REQUIRED CONFIG HINTS "${DIODE_CMAKE_DIR}")

enable_testing()

add_executable(test_simple test_simple.c)
target_include_directories(test_simple PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_simple PRIVATE diode::diode)
cmock_generate(test_simple HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/simple_dep.h)

add_test(NAME test_simple COMMAND test_simple)
