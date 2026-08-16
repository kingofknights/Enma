#include "Sound.hpp"
#include "miniaudio.h"
#include "spdlog/spdlog.h"

ma_engine* AudioEngine::_engine = nullptr;

auto AudioEngine::Initialize() -> bool {
    if (_engine != nullptr) {
        return true;
    }
    
    _engine = new ma_engine();
    ma_result result = ma_engine_init(nullptr, _engine);
    if (result != MA_SUCCESS) {
        spdlog::error("Failed to initialize miniaudio engine");
        delete _engine;
        _engine = nullptr;
        return false;
    }
    
    spdlog::info("AudioEngine initialized successfully");
    return true;
}

auto AudioEngine::Shutdown() -> void {
    if (_engine != nullptr) {
        ma_engine_uninit(_engine);
        delete _engine;
        _engine = nullptr;
    }
}

auto AudioEngine::GetEngine() -> ma_engine* {
    return _engine;
}

Sound::Sound(std::string_view filepath_) {
    _data._soundHandle = nullptr;

    if (AudioEngine::GetEngine() == nullptr) {
        spdlog::warn("Cannot load sound '{}': AudioEngine is not initialized.", filepath_);
        return;
    }

    _data._soundHandle = new ma_sound();
    ma_result result = ma_sound_init_from_file(AudioEngine::GetEngine(), filepath_.data(), 0, nullptr, nullptr, _data._soundHandle);
    if (result != MA_SUCCESS) {
        spdlog::error("Failed to load sound from file: {}", filepath_);
        delete _data._soundHandle;
        _data._soundHandle = nullptr;
    }
}

Sound::~Sound() {
    if (_data._soundHandle != nullptr) {
        ma_sound_uninit(_data._soundHandle);
        delete _data._soundHandle;
        _data._soundHandle = nullptr;
    }
}

auto Sound::Play() -> void {
    if (_data._soundHandle != nullptr) {
        // Seek to the beginning to allow replaying the same sound
        ma_sound_seek_to_pcm_frame(_data._soundHandle, 0);
        ma_sound_start(_data._soundHandle);
    }
}
