cmake_minimum_required(VERSION 3.11)
project(RainApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# Use FetchContent to pull raylib directly from source for ALL platforms
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
    SOURCE_DIR ${VENDOR_DIR}/raylib
)

# Disable examples and games to speed up the build
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

add_executable(RainApp src/main.cpp)

target_link_libraries(RainApp PRIVATE raylib)
target_include_directories(RainApp PRIVATE ${VENDOR_DIR}/raylib/src)

if (EMSCRIPTEN)
    target_compile_definitions(RainApp PRIVATE PLATFORM_WEB)
    set_target_properties(RainApp PROPERTIES SUFFIX ".html")
    
    # Required link options for Raylib on web
    target_link_options(RainApp PRIVATE -s USE_GLFW=3)
    
    # Include raylib's default HTML shell if available
    set(RAYLIB_SHELL "${VENDOR_DIR}/raylib/src/shell.html")
    if(EXISTS ${RAYLIB_SHELL})
        target_link_options(RainApp PRIVATE "--shell-file" "${RAYLIB_SHELL}")
    endif()
endif()
