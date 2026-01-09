//
// Created by nikolaj on 1/6/26.
//

#ifndef COUNTRYBRAWL_MUSICMANAGER_H
#define COUNTRYBRAWL_MUSICMANAGER_H
#include <random>
#include <vector>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#include "musicWrap.h"
#include "texwrap.h"


class musicManager {
public:

    musicManager(SDL_Renderer* renderer, TTF_Font* font);

    void setLoopTitleTrack (bool state) {loopTitleTrack=state;};
    void update(std::default_random_engine& generator);

    [[nodiscard]] int getMusicVolume() const {return musicVolume;}
    void setMusicVolume(int volume);

    void displayManager(SDL_Renderer* renderer, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) const;
    void updateManager(int mouseX, int mouseY, bool leftDown, bool leftClick,int windowWidth, int windowHeight, double scale);
private:
    std::vector<musicWrap> tracks;
    std::vector<texwrap> titleTexts;
    int titleIndex;

    int musicVolume;

    int currentPlayingTrack;

    bool loopTitleTrack;


    int musicVolumeBarWidth = 512;
    int musicVolumeBarHeight = 64;

    texwrap musicSettingsText;
    texwrap musicOffTexture;
    texwrap musicFullTexture;
    texwrap selectedMusicTexture;
};


#endif //COUNTRYBRAWL_MUSICMANAGER_H