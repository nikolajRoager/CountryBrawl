//
// Created by nikolaj on 1/12/26.
//

#ifndef COUNTRYBRAWL_SOUNDWRAP_H
#define COUNTRYBRAWL_SOUNDWRAP_H

#include <filesystem>
#include <SDL2/SDL_mixer.h>

namespace fs = std::filesystem;

class soundWrap {
public:
    soundWrap(const fs::path& path);
    soundWrap(soundWrap&& other) noexcept ;
    soundWrap& operator=(soundWrap&& other) noexcept ;
    ~soundWrap();

    //Play a sound, if the source is on screen
    void play(double x, double y,double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale) const ;

    [[nodiscard]] Mix_Chunk* getSound() const {return sound;}

    void attenuate(double factor) {
        factor = factor * factor * (3.0f - 2.0f * factor);
        Mix_VolumeChunk(sound, std::max(0,std::min( static_cast<int>(factor * MIX_MAX_VOLUME),MIX_MAX_VOLUME)));
    }
private:
    Mix_Chunk* sound;
};


#endif //COUNTRYBRAWL_SOUNDWRAP_H