cmake_minimum_required(VERSION 3.11)
project(RainApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

include(FetchContent)

if(NOT EMSCRIPTEN)
    # Fetch spdlog for asynchronous ultra-low latency logging
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
        SOURCE_DIR ${VENDOR_DIR}/spdlog
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# Use FetchContent to pull raylib directly from source for ALL platforms
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

if(NOT EMSCRIPTEN)
    # libpqxx for PostgreSQL (Native only)
    FetchContent_Declare(
        libpqxx
        GIT_REPOSITORY https://github.com/jtv/libpqxx.git
        GIT_TAG 7.9.0
        SOURCE_DIR ${VENDOR_DIR}/libpqxx
    )
    set(SKIP_BUILD_TEST ON CACHE BOOL "" FORCE)
    set(BUILD_DOC OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(libpqxx)
endif()

add_executable(RainApp 
    src/main.cpp
    src/core/network/UdpReceiver.cpp
    src/core/network/TcpConnection.cpp
    src/core/database/PostgresConnection.cpp
)

target_link_libraries(RainApp PRIVATE raylib)

if(NOT EMSCRIPTEN)
    target_link_libraries(RainApp PRIVATE pqxx spdlog)
    
    # Ensure assets (like fonts) are copied to the build directory for native execution
    add_custom_command(TARGET RainApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
    )
endif()

if(WIN32)
    target_link_libraries(RainApp PRIVATE ws2_32)
endif()
target_include_directories(RainApp PRIVATE 
    ${VENDOR_DIR}/raylib/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (EMSCRIPTEN)
    target_compile_definitions(RainApp PRIVATE PLATFORM_WEB)
    set_target_properties(RainApp PROPERTIES SUFFIX ".html")
    
    # Required link options for Raylib on web
    target_link_options(RainApp PRIVATE -s USE_GLFW=3)
    
    # Preload the assets directory (for Material Icons)
    target_link_options(RainApp PRIVATE "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
    
    # Include raylib's default HTML shell if available
    set(RAYLIB_SHELL "${VENDOR_DIR}/raylib/src/shell.html")
    if(EXISTS ${RAYLIB_SHELL})
        target_link_options(RainApp PRIVATE "--shell-file" "${RAYLIB_SHELL}")
    endif()
endif()
