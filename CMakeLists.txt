cmake_minimum_required(VERSION 3.20)

project(DPFixNG VERSION 0.0.38 LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "DPFix-NG currently supports Windows only")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Deadly Premonition is 32-bit. Configure with -A Win32.")
endif()

include(FetchContent)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG v1.3.4
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(minhook)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.9b
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(imgui)

set(DPFIXNG_IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
)

add_library(DPFixNG MODULE
    src/asi_main.cpp

    src/dpfixng/logging.cpp
    src/dpfixng/config.cpp
    src/dpfixng/main_exe.cpp
    src/dpfixng/world_streaming.cpp
    src/dpfixng/ui_settings.cpp

    ${DPFIXNG_IMGUI_SOURCES}
)

# Allows includes such as:
#   #include "dpfixng/config.h"
# from src/asi_main.cpp and future modules.
target_include_directories(DPFixNG PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_compile_features(DPFixNG PRIVATE cxx_std_20)

target_compile_definitions(DPFixNG PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

target_link_libraries(DPFixNG PRIVATE
    minhook
)

if(MSVC)
    target_compile_options(DPFixNG PRIVATE /W4 /permissive-)
    set_source_files_properties(${DPFIXNG_IMGUI_SOURCES} PROPERTIES COMPILE_OPTIONS "/W0")
endif()

set_target_properties(DPFixNG PROPERTIES
    PREFIX ""
    OUTPUT_NAME "DPFixNG"
    SUFFIX ".asi"
)
