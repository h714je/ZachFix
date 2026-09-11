cmake_minimum_required(VERSION 3.20)

project(DPFixNG VERSION 0.0.4 LANGUAGES C CXX)

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

add_library(DPFixNG MODULE
    src/asi_main.cpp
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
endif()

set_target_properties(DPFixNG PROPERTIES
    PREFIX ""
    OUTPUT_NAME "DPFixNG"
    SUFFIX ".asi"
)