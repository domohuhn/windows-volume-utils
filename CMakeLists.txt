cmake_minimum_required(VERSION 3.14)
project(volume_utils VERSION 1.0.0)

option(BUILD_VOLUME_UTILS_TESTS "Controls if the test apps are built" OFF)
option(BUILD_VOLUME_UTILS_APP "Controls if the gui app is built" OFF)

add_library(volume_utils_lib
    include/audio_volume.hpp
    src/audio_volume.cpp
)
target_include_directories(volume_utils_lib
PUBLIC
    include
)


#
# UI app demostrating the use of the class
#

if(BUILD_VOLUME_UTILS_APP)
    include(FetchContent)
    set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
    FetchContent_Declare(
        borderless_window
        GIT_REPOSITORY https://github.com/cazzwastaken/borderless-imgui-window
        GIT_TAG        main
        PATCH_COMMAND  git apply ${CMAKE_CURRENT_SOURCE_DIR}/app/fix_return_type.patch
    )
    FetchContent_GetProperties(borderless_window)
    if(NOT EXISTS ${borderless_window_SOURCE_DIR})
        message(STATUS "Populating imgui borderless window")
        FetchContent_Populate(borderless_window)
    endif()
    add_library(gui_window
        ${borderless_window_SOURCE_DIR}/cheat/gui.cpp
        ${borderless_window_SOURCE_DIR}/imgui/imgui.cpp
        ${borderless_window_SOURCE_DIR}/imgui/imgui_demo.cpp
        ${borderless_window_SOURCE_DIR}/imgui/imgui_draw.cpp
        ${borderless_window_SOURCE_DIR}/imgui/imgui_impl_dx9.cpp
        ${borderless_window_SOURCE_DIR}/imgui/imgui_impl_win32.cpp
        ${borderless_window_SOURCE_DIR}/imgui/imgui_tables.cpp
        ${borderless_window_SOURCE_DIR}/imgui/imgui_widgets.cpp
    )
    target_include_directories(gui_window PUBLIC ${borderless_window_SOURCE_DIR})
    target_compile_features(gui_window PUBLIC cxx_std_17)

    add_library(keyboard_hook SHARED app/keyboard_hook.cpp)
    target_compile_features(keyboard_hook PUBLIC cxx_std_17)

    add_executable(volume_utils WIN32
        app/main.cpp
        app/keyboard_listener.cpp
    )
    target_include_directories(volume_utils PUBLIC ${borderless_window_SOURCE_DIR})
    target_link_libraries(volume_utils PRIVATE gui_window volume_utils_lib)

endif()

#
# Testing apps
#

if(BUILD_VOLUME_UTILS_TESTS)
    add_executable(test_toggle_mute tests/test_toggle_mute.cpp)
    target_link_libraries(test_toggle_mute PRIVATE volume_utils_lib)

    add_executable(test_set_volume tests/test_set_volume.cpp)
    target_link_libraries(test_set_volume PRIVATE volume_utils_lib)
endif()
