//
// Created by nikolaj on 1/6/26.
//

#include "musicManager.h"

#include <fstream>
#include <iostream>

#include "getAssets.h"
#include "thirdPartyLibraryInclude/json.hpp"

struct musicInfo {
    bool isTitleTrack=false;
    std::string name="null";
    std::string file="null";
};

/* Example of valid Json content
[
  {"name": "Den Tabre Landssoldat",
  "file": "Dengang_jeg_drog_af_sted.ogg",
  "title": true},
  {"name": "Die Wacht am Rhein",
  "file": "Die_Wacht_am_Rhein.wav"},
  {"name": "Preußens Gloria",
  "file": "PreussensGloria.mp3"}
]
 */

void from_json(const nlohmann::json& j, musicInfo& m) {
    m.isTitleTrack = j.value("title", false);
    j.at("name").get_to(m.name);
    j.at("file").get_to(m.file);
}

musicManager::musicManager() {
    std::cout<<"Loading music"<<std::endl;
    titleIndex=0;
    fs::path musicJsonFile = assetsPath()/"Music"/"music.json";

    std::ifstream file(musicJsonFile);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open "+musicJsonFile.string());
    }
    nlohmann::json result;
    file>>result;

    auto musicInfoVector= result.get<std::vector<musicInfo>>();

    for (int i = 0; i < musicInfoVector.size(); i++) {
        tracks.emplace_back(assetsPath()/"Music"/musicInfoVector[i].file,musicInfoVector[i].name);
        if (musicInfoVector[i].isTitleTrack) {
            titleIndex=i;
        }
    }
    currentPlayingTrack=titleIndex;
    loopTitleTrack=false;

    musicVolume=Mix_VolumeMusic(-1);

}

void musicManager::update(std::default_random_engine& generator) {
    //We only need to do something, if music has stopped playing
    if (Mix_PlayingMusic()!=1) {
        if (loopTitleTrack) {
            //I play without loops, so I can control what track we plays next
            Mix_PlayMusic(tracks[titleIndex].getMusic(),0);
        }
        else {
            std::uniform_int_distribution<int> distribution(0, tracks.size()-1);
            int nextTrack = distribution(generator);
            //I play without loops, so I can control what track we plays next
            Mix_PlayMusic(tracks[nextTrack].getMusic(),0);
        }
    }
}

void musicManager::setMusicVolume(int volume) {
    if (volume!=musicVolume) {
        musicVolume=std::min(std::max(volume,0),SDL_MIX_MAXVOLUME);
        Mix_VolumeMusic(musicVolume);
    }
}
