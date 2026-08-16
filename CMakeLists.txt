cmake_minimum_required(VERSION 3.11)
project(RainApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/imgui.cmake)
include(cmake/Dependencies.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Optimizations.cmake)

add_executable(RainApp 
    src/main.cpp
    src/Dashboard.cpp
    src/audio/MiniaudioImpl.cpp
    src/audio/Sound.cpp
    src/core/network/UdpReceiver.cpp
    src/core/network/TcpConnection.cpp
    src/core/database/PostgresConnection.cpp
)

target_link_libraries(RainApp PRIVATE imgui)

if(NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    target_link_libraries(RainApp PRIVATE pqxx spdlog OpenGL::GL)
    
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
target_include_directories(RainApp PRIVATE src)
target_include_directories(RainApp SYSTEM PRIVATE
    ${spdlog_SOURCE_DIR}/include
    ${libpqxx_SOURCE_DIR}/include
    ${iconfontcppheaders_SOURCE_DIR}
    ${miniaudio_SOURCE_DIR}
)

# Enable project warnings and LTO optimization
set_project_warnings(RainApp)
enable_project_optimizations(RainApp)

# Precompile headers to significantly reduce compilation times
target_precompile_headers(RainApp PRIVATE
    <cstdint>
    <string>
    <string_view>
    <vector>
    <array>
    <memory>
    <utility>
)

if(NOT EMSCRIPTEN)
    target_precompile_headers(RainApp PRIVATE <spdlog/spdlog.h>)
endif()

if (EMSCRIPTEN)
    target_compile_definitions(RainApp PRIVATE PLATFORM_WEB)
    set_target_properties(RainApp PROPERTIES SUFFIX ".html")
    
    # Required link options for GLFW on web
    target_link_options(RainApp PRIVATE -s USE_GLFW=3)
    
    # Preload the assets directory (for Material Icons)
    target_link_options(RainApp PRIVATE "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
endif()
