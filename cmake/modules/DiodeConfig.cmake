# DiodeConfig.cmake — top-level entry for CMake consumers (plain or Zephyr).
#
# Typical use:
#
#   execute_process(COMMAND diode cmake-dir
#                   OUTPUT_VARIABLE DIODE_CMAKE_DIR
#                   OUTPUT_STRIP_TRAILING_WHITESPACE)
#   find_package(Diode REQUIRED CONFIG HINTS "${DIODE_CMAKE_DIR}")
#
#   add_executable(my_test my_test.c)
#   target_link_libraries(my_test PRIVATE diode::diode)  # gets unity too
#   add_test(NAME my_test COMMAND my_test)
#
# If the test mocks platform calls:
#
#   cmock_generate(my_test HEADERS pal_net.h)
#
# Provides:
#   - diode::diode  — header-only today; PUBLIC-links unity::framework so
#                     anything linking diode also gets Unity headers and lib
#   - cmock_generate(target HEADERS ...) via FindCMock
#
# Resolution: this file lives at <pkg>/cmake/modules/, so the diode headers
# live two directories up at <pkg>/include/. We resolve relative to
# CMAKE_CURRENT_LIST_DIR so this works from a wheel install, an editable
# install, or a raw checkout.

if(TARGET diode::diode)
    return()
endif()

set(_diode_modules_dir ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(_diode_root ${_diode_modules_dir}/../.. ABSOLUTE)
set(_diode_include_dir ${_diode_root}/include)

if(NOT EXISTS ${_diode_include_dir}/diode/common/list.h)
    message(FATAL_ERROR
        "DiodeConfig: cannot locate diode headers (looked at "
        "${_diode_include_dir}). The package layout is broken.")
endif()

list(APPEND CMAKE_MODULE_PATH ${_diode_modules_dir})

find_package(Unity REQUIRED)
find_package(CMock REQUIRED)

add_library(diode INTERFACE)
add_library(diode::diode ALIAS diode)
target_include_directories(diode INTERFACE ${_diode_include_dir})
target_compile_features(diode INTERFACE c_std_11)
target_link_libraries(diode INTERFACE unity::framework)

set(Diode_FOUND TRUE)
