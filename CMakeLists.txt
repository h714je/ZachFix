cmake_minimum_required(VERSION 3.20)

project(ZachFix VERSION 0.2.1 LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "ZachFix currently supports Windows only")
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

# MinHook is linked into ZachFix, but its own install rules are SDK-oriented
# (headers, static library, CMake package files). Keep those out of the
# end-user runtime package while still building minhook as a dependency.
set_property(DIRECTORY "${minhook_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL TRUE)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.9b
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    miniz
    GIT_REPOSITORY https://github.com/richgel999/miniz.git
    GIT_TAG 3.0.2
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR _zachfix_no_cmake
)

# ZachFix only needs miniz's small ZIP/Deflate implementation. Populate the
# pinned source without adopting miniz's own project/install rules.
FetchContent_MakeAvailable(miniz)

set(ZACHFIX_MINIZ_EXPORT_DIR "${CMAKE_CURRENT_BINARY_DIR}/miniz_compat")
file(MAKE_DIRECTORY "${ZACHFIX_MINIZ_EXPORT_DIR}")
file(WRITE "${ZACHFIX_MINIZ_EXPORT_DIR}/miniz_export.h"
    "#pragma once\n#define MINIZ_EXPORT\n")

add_library(zachfix_miniz STATIC
    ${miniz_SOURCE_DIR}/miniz.c
    ${miniz_SOURCE_DIR}/miniz_zip.c
    ${miniz_SOURCE_DIR}/miniz_tinfl.c
    ${miniz_SOURCE_DIR}/miniz_tdef.c
)

target_include_directories(zachfix_miniz PUBLIC
    ${miniz_SOURCE_DIR}
    ${ZACHFIX_MINIZ_EXPORT_DIR}
)

if(MSVC)
    target_compile_options(zachfix_miniz PRIVATE /W0)
endif()

set(ZACHFIX_IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
)

add_library(ZachFix MODULE
    src/asi_main.cpp

    src/zachfix/logging.cpp
    src/zachfix/config.cpp
    src/zachfix/main_exe.cpp
    src/zachfix/save_diag.cpp
    src/zachfix/difficulty.cpp
    src/zachfix/vanilla_nan_fix.cpp
    src/zachfix/world_streaming.cpp
    src/zachfix/house_list_fix.cpp
    src/zachfix/native_xinput.cpp
    src/zachfix/input_mode.cpp
    src/zachfix/gameplay_pause.cpp
    src/zachfix/ui_settings.cpp
    src/zachfix/texture_override.cpp

    ${ZACHFIX_IMGUI_SOURCES}
)

# Allows includes such as:
#   #include "zachfix/config.h"
# from src/asi_main.cpp and future modules.
target_include_directories(ZachFix PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_compile_features(ZachFix PRIVATE cxx_std_20)

# Let Release builds optimize across ZachFix translation-unit boundaries. This
# is especially useful for the thin D3D9 hooks that call small helpers living
# in texture/input modules, while keeping Debug builds fast to iterate on.
include(CheckIPOSupported)
check_ipo_supported(RESULT ZACHFIX_IPO_SUPPORTED OUTPUT ZACHFIX_IPO_ERROR LANGUAGES CXX)
if(ZACHFIX_IPO_SUPPORTED)
    set_property(TARGET ZachFix PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

target_compile_definitions(ZachFix PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

target_link_libraries(ZachFix PRIVATE
    minhook
    zachfix_miniz
    Psapi
)

if(MSVC)
    target_compile_options(ZachFix PRIVATE /W4 /permissive-)
    set_source_files_properties(${ZACHFIX_IMGUI_SOURCES} PROPERTIES COMPILE_OPTIONS "/W0")
endif()

set_target_properties(ZachFix PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ZachFix"
    SUFFIX ".asi"
)

# Release packaging
install(TARGETS ZachFix
    RUNTIME DESTINATION scripts
    LIBRARY DESTINATION scripts
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ZachFix.ini
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    ${CMAKE_CURRENT_SOURCE_DIR}/THIRD_PARTY.md
    DESTINATION .
)

install(FILES
    ${minhook_SOURCE_DIR}/LICENSE.txt
    DESTINATION licenses
    RENAME MinHook.txt
)

install(FILES
    ${imgui_SOURCE_DIR}/LICENSE.txt
    DESTINATION licenses
    RENAME DearImGui.txt
)

install(FILES
    ${miniz_SOURCE_DIR}/LICENSE
    DESTINATION licenses
    RENAME miniz.txt
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/licenses/SuperFastHash.txt
    DESTINATION licenses
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/licenses/Kenney-Input-Prompts.txt
    DESTINATION licenses
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/
    DESTINATION docs
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/ZachFix/glyphs/
    DESTINATION ZachFix/glyphs
    PATTERN ".gitkeep" EXCLUDE
)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "ZachFix")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "ZachFix-v${PROJECT_VERSION}-win32")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
