//
// Created by nikolaj on 1/12/26.
//

#include "soundWrap.h"

#include <iostream>

soundWrap::soundWrap(const fs::path &path) {
    sound = Mix_LoadWAV(path.string().c_str());
    if (!sound) {
        throw std::runtime_error("Could not load sound file: " + path.string()+" error: " + std::string(Mix_GetError()));
    }
}

soundWrap &soundWrap::operator=(soundWrap &&other) noexcept {
    sound=other.sound;
    other.sound = nullptr;
    return *this;
}

soundWrap::soundWrap(soundWrap &&other) noexcept {
    sound=other.sound;
    other.sound = nullptr;
}

soundWrap::~soundWrap() {
    if (sound!=nullptr) {
        Mix_FreeChunk(sound);
    }
}

void soundWrap::play(double x, double y, double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale,bool silenceAtScaleout) const {

    int xScreen = static_cast<int>(x*scale-screenMinX);
    int yScreen = static_cast<int>(y*scale-screenMinY);

    if (xScreen<0 || xScreen>screenWidth|| yScreen<0 || yScreen>screenHeight || (silenceAtScaleout && scale<0.5)) {
        return;
    }

    if (sound!=nullptr) {
        Mix_PlayChannel(-1, sound, 0);
    }
}


