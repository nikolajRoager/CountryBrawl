//
// Created by nikolaj on 12/24/25.
//

#ifndef COUNTRYBRAWL_ENGINE_H
#define COUNTRYBRAWL_ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "scene.h"
#include "texwrap.h"

class engine {
public:
    engine();
    ~engine();
    void run();

private:

    //The window we'll be rendering to
    SDL_Window* window;

    //The window renderer
    SDL_Renderer* renderer;

    TTF_Font* smallFont;
    TTF_Font* midFont;
    TTF_Font* largeFont;

    //This texture is loaded by the engine not the individual scenarios, so that it is always available, and never unloaded until the end of the program
    //I use a pointer, because it must not be created before SDL is initialized, which a non-pointer would be
    texwrap* loadingBackground;

    scene* currentScene;

    int windowWidthPx;
    int windowHeightPx;
};


#endif //COUNTRYBRAWL_ENGINE_H