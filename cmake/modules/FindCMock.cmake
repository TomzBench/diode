# FindCMock.cmake
#
# Find or fetch the ThrowTheSwitch CMock mock-generation framework.
#
# CMock has two parts:
#   1. A small C runtime (src/cmock.c, src/cmock_internals.h) that mocks link
#      against. Exposed here as the cmock::cmock target.
#   2. A Ruby code generator (lib/cmock.rb) that turns a header file into a
#      mock implementation. Exposed via the cmock_generate() helper.
#
# Options:
#   CMOCK_VERSION       - Git tag to fetch (default: v2.6.0)
#   CMOCK_FIND_REQUIRED - If TRUE, failure is fatal (standard find_package
#                         behavior — set automatically by find_package(... REQUIRED))
#
# Imported targets:
#   cmock::cmock        - Link this into test binaries to get the CMock runtime.
#                         Brings Unity along (CMock depends on Unity).
#
# Functions:
#   cmock_generate(<target> HEADERS <h1> [<h2> ...] [CONFIG <yml>] [OUTDIR <dir>])
#       Runs ruby lib/cmock.rb on each header, producing Mock<header>.c/.h
#       under OUTDIR (default: ${CMAKE_CURRENT_BINARY_DIR}/mocks). Adds the
#       generated .c files as PRIVATE sources of <target> and prepends OUTDIR
#       to its include path. CONFIG is an optional CMock YAML config file.

if(TARGET cmock::cmock)
    return()
endif()

# CMock depends on Unity at runtime — pull it in first so the cmock library
# can link against unity::framework.
find_package(Unity REQUIRED)

# Try system-installed CMock first. CMock does not historically ship a CMake
# config, but distro packages or Ceedling-bundled installs sometimes do —
# check anyway, it's cheap.
find_package(cmock CONFIG QUIET)
if(cmock_FOUND AND TARGET cmock::cmock)
    set(CMock_FOUND TRUE)
    return()
endif()

# Ruby is required for the generator. Fail loudly here rather than during
# the first cmock_generate() call.
find_program(CMOCK_RUBY_EXECUTABLE ruby)
if(NOT CMOCK_RUBY_EXECUTABLE)
    if(CMock_FIND_REQUIRED)
        message(FATAL_ERROR
            "FindCMock: ruby not found on PATH. CMock's mock generator "
            "requires Ruby. Install Ruby >=2.7 and ensure it is on PATH.")
    endif()
    set(CMock_FOUND FALSE)
    return()
endif()

include(FetchContent)

if(NOT DEFINED CMOCK_VERSION)
    set(CMOCK_VERSION "v2.6.0")
endif()

# CMock has no top-level CMakeLists.txt — fetch sources only.
FetchContent_Declare(
    cmock
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/CMock.git
    GIT_TAG ${CMOCK_VERSION}
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cmock)

# Build the CMock C runtime ourselves (no upstream CMakeLists).
add_library(cmock STATIC ${cmock_SOURCE_DIR}/src/cmock.c)
target_include_directories(cmock PUBLIC ${cmock_SOURCE_DIR}/src)
target_link_libraries(cmock PUBLIC unity::framework)
set_target_properties(cmock PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)
add_library(cmock::cmock ALIAS cmock)

# Path to the Ruby generator script — referenced by cmock_generate().
set(CMOCK_GENERATOR_SCRIPT "${cmock_SOURCE_DIR}/lib/cmock.rb"
    CACHE FILEPATH "Path to cmock.rb")

# Suppress MSVC Spectre mitigation warnings on CMock sources.
if(MSVC AND TARGET cmock)
    target_compile_options(cmock PRIVATE /wd5045)
endif()

# cmock_generate(<target> HEADERS h1 [h2 ...] [CONFIG yml] [OUTDIR dir])
#
# Runs cmock.rb to generate mock C/H pairs from the listed headers and
# adds the generated .c files as PRIVATE sources of <target>. Generated
# files land in OUTDIR (default: ${CMAKE_CURRENT_BINARY_DIR}/mocks).
function(cmock_generate target)
    cmake_parse_arguments(ARG "" "CONFIG;OUTDIR" "HEADERS" ${ARGN})

    if(NOT ARG_HEADERS)
        message(FATAL_ERROR "cmock_generate: HEADERS is required")
    endif()
    if(NOT ARG_OUTDIR)
        set(ARG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/mocks")
    endif()
    file(MAKE_DIRECTORY ${ARG_OUTDIR})

    set(_cmock_args --mock_path=${ARG_OUTDIR})
    if(ARG_CONFIG)
        list(APPEND _cmock_args -o${ARG_CONFIG})
    endif()

    set(_generated_sources)
    foreach(header IN LISTS ARG_HEADERS)
        get_filename_component(_stem ${header} NAME_WE)
        set(_mock_c "${ARG_OUTDIR}/Mock${_stem}.c")
        set(_mock_h "${ARG_OUTDIR}/Mock${_stem}.h")
        get_filename_component(_header_abs ${header} ABSOLUTE)

        add_custom_command(
            OUTPUT ${_mock_c} ${_mock_h}
            COMMAND ${CMOCK_RUBY_EXECUTABLE}
                    ${CMOCK_GENERATOR_SCRIPT}
                    ${_cmock_args}
                    ${_header_abs}
            DEPENDS ${_header_abs} ${CMOCK_GENERATOR_SCRIPT} ${ARG_CONFIG}
            COMMENT "CMock: generating Mock${_stem} from ${header}"
            VERBATIM
        )
        list(APPEND _generated_sources ${_mock_c})
    endforeach()

    target_sources(${target} PRIVATE ${_generated_sources})
    target_include_directories(${target} PRIVATE ${ARG_OUTDIR})
    target_link_libraries(${target} PRIVATE cmock::cmock)
endfunction()

set(CMock_FOUND TRUE)
