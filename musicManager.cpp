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

musicManager::musicManager(SDL_Renderer* renderer, TTF_Font* font):
musicSettingsText("Music settings, esc to close",renderer,font),musicOffTexture(assetsPath()/"ui"/"menuMusicOff.png",renderer),musicFullTexture(assetsPath()/"ui"/"menuMusic.png",renderer),
selectedMusicTexture(assetsPath()/"ui"/"selectedSong.png",renderer)
{
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
        titleTexts.emplace_back(musicInfoVector[i].name,renderer,font);
    }
    currentPlayingTrack=titleIndex;
    loopTitleTrack=false;

    musicVolume=Mix_VolumeMusic(-1);

    musicVolumeBarWidth = 512;
    musicVolumeBarHeight = 64;
}

void musicManager::update(std::default_random_engine& generator) {
    //We only need to do something, if music has stopped playing
    if (Mix_PlayingMusic()!=1) {
        if (loopTitleTrack) {
            //I play without loops, so I can control what track we plays next
            Mix_PlayMusic(tracks[titleIndex].getMusic(),0);
            currentPlayingTrack=titleIndex;
        }
        else {
            std::uniform_int_distribution<int> distribution(0, tracks.size()-1);
            int nextTrack = distribution(generator);
            //I play without loops, so I can control what track we plays next
            Mix_PlayMusic(tracks[nextTrack].getMusic(),0);
            currentPlayingTrack=nextTrack;
        }
    }
}

void musicManager::setMusicVolume(int volume) {
    if (volume!=musicVolume) {
        musicVolume=std::min(std::max(volume,0),SDL_MIX_MAXVOLUME);
        Mix_VolumeMusic(musicVolume);
    }
}

void musicManager::displayManager(SDL_Renderer *renderer, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) const {
    int middleScreenX = windowWidth/ 2;
    int topScreenY = windowHeight/ 4;
    musicSettingsText.render(middleScreenX,topScreenY,renderer,scale,true,true);
    int musicVolumeBarX0 = middleScreenX-(int)(scale*musicVolumeBarWidth/2);
    int musicVolumeBarY0 = topScreenY;

    SDL_Rect totalBackground {musicVolumeBarX0,musicVolumeBarY0,(int)(scale*musicVolumeBarWidth),windowHeight/2};
    SDL_SetRenderDrawColor(renderer,64,64,64,255);
    SDL_RenderFillRect(renderer,&totalBackground);

    SDL_Rect musicVolumeBarQuad{musicVolumeBarX0 ,musicVolumeBarY0,(int)(scale*musicVolumeBarWidth),(int)(scale*musicVolumeBarHeight)};
    SDL_SetRenderDrawColor(renderer,128,128,128,255);
    SDL_RenderFillRect(renderer,&musicVolumeBarQuad);
    for (int i = 0; i < musicVolume; ++i) {
        //Draw green quads at 3 px wide, for each volume level
        SDL_Rect quad{(int)(musicVolumeBarX0+scale*i*musicVolumeBarWidth/SDL_MIX_MAXVOLUME),musicVolumeBarY0, std::max(1,(int)(scale*musicVolumeBarWidth/SDL_MIX_MAXVOLUME-1)),(int)(scale*musicVolumeBarHeight)};
        SDL_SetRenderDrawColor(renderer,128,255,128,255);
        SDL_RenderFillRect(renderer,&quad);
    }
    musicOffTexture.render(musicVolumeBarX0-scale*musicOffTexture.getWidth(),musicVolumeBarY0,renderer,scale);
    musicFullTexture.render(musicVolumeBarX0+scale*musicVolumeBarWidth,musicVolumeBarY0,renderer,scale);
    int y =musicVolumeBarY0+musicOffTexture.getHeight();
    for (int i = 0; i < titleTexts.size(); i++) {
        bool mouseOVer=
            (mouseX>musicVolumeBarX0 && mouseX<musicVolumeBarX0+scale*musicVolumeBarWidth &&
            mouseY>y && mouseY<y+scale*titleTexts[i].getHeight());
        int tint = mouseOVer ? 128 : 255;
                titleTexts[i].render(musicVolumeBarX0 ,y,tint ,tint ,tint ,renderer,scale);
        if (i == currentPlayingTrack) {
            selectedMusicTexture.render(musicVolumeBarX0-selectedMusicTexture.getWidth()*scale,y,renderer,scale);
        }
        y+=(titleTexts[i].getHeight())*scale+4;
    }
}

void musicManager::updateManager(int mouseX, int mouseY, bool leftDown, bool leftClick, int windowWidth, int windowHeight, double scale) {

    //Check if we clicked the bar which controls music volume
    int middleScreenX = windowWidth/ 2;
    int topScreenY = windowHeight/ 4;
    int musicVolumeBarX0 = middleScreenX-scale*musicVolumeBarWidth/2;
    int musicVolumeBarY0 = topScreenY;
    if (leftDown) {
        int newVolume=-1;
        if (mouseY>musicVolumeBarY0 && mouseY<musicVolumeBarY0+scale*musicVolumeBarHeight) {
            if (mouseX>musicVolumeBarX0 && mouseX<musicVolumeBarX0+scale*musicVolumeBarWidth) {
                newVolume= (mouseX-musicVolumeBarX0)/(scale*musicVolumeBarWidth/SDL_MIX_MAXVOLUME);
            }
            else if (mouseX<musicVolumeBarX0 && mouseX>musicVolumeBarX0-scale*musicOffTexture.getWidth()) {
                newVolume=0;
            }
            else if (mouseX<musicVolumeBarX0+scale*musicVolumeBarWidth+scale*musicOffTexture.getWidth() && mouseX>musicVolumeBarX0+scale*musicVolumeBarWidth) {
                newVolume=SDL_MIX_MAXVOLUME;
            }
        }
        if (newVolume!=-1)
            setMusicVolume(newVolume);
    }
    if (leftClick) {
        int y =musicVolumeBarY0+musicOffTexture.getHeight();
        for (int i = 0; i < titleTexts.size(); i++) {
            if (mouseX>musicVolumeBarX0 && mouseX<musicVolumeBarX0+scale*musicVolumeBarWidth &&
            mouseY>y && mouseY<y+scale*titleTexts[i].getHeight()) {
                //I play without loops, so I can control what track we plays next
                Mix_PlayMusic(tracks[i].getMusic(),0);
                currentPlayingTrack=i;
            }
            y+=(titleTexts[i].getHeight())*scale+4;
        }
    }
}
