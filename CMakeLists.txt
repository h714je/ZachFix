cmake_minimum_required(VERSION 3.20)

project(ZachFix VERSION 0.1.0 LANGUAGES C CXX)

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
    src/zachfix/vanilla_nan_fix.cpp
    src/zachfix/world_streaming.cpp
    src/zachfix/gameplay_pause.cpp
    src/zachfix/ui_settings.cpp
    src/zachfix/texture_override.cpp
    src/zachfix/shader_probe.cpp

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

target_compile_definitions(ZachFix PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

target_link_libraries(ZachFix PRIVATE
    minhook
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
    ${CMAKE_CURRENT_SOURCE_DIR}/licenses/SuperFastHash.txt
    DESTINATION licenses
)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "ZachFix")
set(CPACK_PACKAGE_VERSION "0.1.0-rc4")
set(CPACK_PACKAGE_FILE_NAME "ZachFix-v0.1.0-rc4-win32")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
