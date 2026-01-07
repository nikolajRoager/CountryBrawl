//
// Created by nikolaj on 1/6/26.
//

#include "musicWrap.h"

musicWrap::musicWrap(const fs::path& path, const std::string& _name) {

    music = Mix_LoadMUS(path.string().c_str());
    if (!music) {
        throw std::runtime_error("Could not load music file: " + path.string()+" error: " + std::string(Mix_GetError()));
    }
    length = Mix_GetMusicLoopLengthTime(music);
    name = _name;
}

musicWrap::musicWrap(musicWrap &&tex) noexcept {
    music=tex.music;
    tex.music=nullptr;
    length=tex.length;
    name=tex.name;
}

musicWrap& musicWrap::operator=(musicWrap &&tex) noexcept {
    music=tex.music;
    tex.music=nullptr;
    length=tex.length;
    name=tex.name;
    return *this;
}

musicWrap::~musicWrap() {
    if (music!=nullptr)
        Mix_FreeMusic(music);
}
