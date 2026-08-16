#pragma once

#include <string_view>

struct ma_engine;
struct ma_sound;

class AudioEngine {
public:
    static auto Initialize() -> bool;
    static auto Shutdown() -> void;
    static auto GetEngine() -> ma_engine*;

private:
    static ma_engine* _engine;
};

class Sound {
public:
    Sound(std::string_view filepath_);
    ~Sound();

    // Disable copy/move to prevent double-freeing the miniaudio resources
    Sound(const Sound&) = delete;
    auto operator=(const Sound&) -> Sound& = delete;

    auto Play() -> void;

private:
    struct PrivateData {
        ma_sound* _soundHandle = nullptr;
    };

    PrivateData _data;
};
