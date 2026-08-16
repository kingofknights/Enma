include(FetchContent)

if(NOT EMSCRIPTEN)
    # Fetch spdlog for asynchronous ultra-low latency logging
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
    )
    FetchContent_MakeAvailable(spdlog)

    # libpqxx for PostgreSQL (Native only)
    FetchContent_Declare(
        libpqxx
        GIT_REPOSITORY https://github.com/jtv/libpqxx.git
        GIT_TAG 7.9.0
    )
    set(SKIP_BUILD_TEST ON CACHE BOOL "" FORCE)
    set(BUILD_DOC OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(libpqxx)
endif()

# Fetch IconFontCppHeaders
FetchContent_Declare(
    IconFontCppHeaders
    GIT_REPOSITORY https://github.com/juliettef/IconFontCppHeaders.git
    GIT_TAG main
)
FetchContent_GetProperties(IconFontCppHeaders)
if(NOT IconFontCppHeaders_POPULATED)
    FetchContent_Populate(IconFontCppHeaders)
endif()

# Fetch miniaudio (Single Header Audio Library)
FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG master
)
FetchContent_GetProperties(miniaudio)
if(NOT miniaudio_POPULATED)
    FetchContent_Populate(miniaudio)
endif()
