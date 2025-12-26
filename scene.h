//
// Created by nikolaj on 12/24/25.
//

#ifndef COUNTRYBRAWL_SCENE_H
#define COUNTRYBRAWL_SCENE_H

#include<SDL2/SDL.h>

#include "inputData.h"
#include "texwrap.h"

///A scene which can be displayed by the engine, this is a purely virtual base class
class scene {
public:
    enum openSceneCommand {MENU,NEW_GAME,LOAD_GAME,QUIT};

    scene() =default;
    virtual ~scene()=default;

    virtual void render(SDL_Renderer* renderer, const texwrap& loadingBackground,int screenWidth, int screenHeight,const inputData& userInputs, unsigned int millis, unsigned int pmillis) const=0;
    virtual void update(SDL_Renderer* renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight,const inputData& userInputs,  unsigned int millis, unsigned int dmillis,TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont)=0;

    ///Returns true if we should open a new scene, the newSceneType is what scene, and the string is the arguments to its constructor (likely the country we will be playing as, or the filename we will be opening)
    virtual bool shouldOpenNewScene(openSceneCommand& command, std::string& arguments) const=0;
private:
};


#endif //COUNTRYBRAWL_SCENE_H