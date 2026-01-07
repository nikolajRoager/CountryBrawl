//
// Created by nikolaj on 1/6/26.
//

#ifndef COUNTRYBRAWL_MUSICMANAGER_H
#define COUNTRYBRAWL_MUSICMANAGER_H
#include <random>
#include <vector>

#include "musicWrap.h"


class musicManager {
public:

    musicManager();

    void setLoopTitleTrack (bool state) {loopTitleTrack=state;};
    void update(std::default_random_engine& generator);

    [[nodiscard]] int getMusicVolume() const {return musicVolume;}
    void setMusicVolume(int volume);
private:
    std::vector<musicWrap> tracks;
    int titleIndex;

    int musicVolume;

    int currentPlayingTrack;

    bool loopTitleTrack;
};


#endif //COUNTRYBRAWL_MUSICMANAGER_H