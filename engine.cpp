//
// Created by nikolaj on 12/24/25.
//

#include "engine.h"
#include "getAssets.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <filesystem>

#include "game.h"
#include "inputData.h"
#include "startMenu.h"

namespace fs = std::filesystem;

engine::engine() {
    window = nullptr;
    renderer = nullptr;
    smallFont= nullptr;

    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        throw std::runtime_error( "SDL_Init Error: " + std::string( SDL_GetError() ) );
    }

    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    windowWidthPx = DM.w;
    windowHeightPx = DM.h;


    window = SDL_CreateWindow( "I can write whatever I want here, and nobody can stop me! HA HA HA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidthPx, windowHeightPx, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
    if( window == nullptr)
        throw std::runtime_error( "Window could not be created! SDL Error:" + std::string( SDL_GetError() ) );
    //Create vsynced renderer for window
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( renderer == nullptr)
    {
        throw std::runtime_error( "Renderer could not be created! SDL Error: "+ std::string(SDL_GetError()) );
    }

    //Initialize renderer color
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        throw std::runtime_error( "SDL_image could not initialize! SDL Error:" + std::string( SDL_GetError() ) );
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        throw std::runtime_error( "SDL_mixer could not initialize! SDL Error:" + std::string( Mix_GetError() ) );
    }


    if (TTF_Init()==-1) {
        throw std::runtime_error( "SDL_TTF could not initialize! SDL Error:" + std::string( SDL_GetError() ) );
    }

    fs::path path = assetsPath()/"AtkinsonHyperlegible-Regular.ttf";
    smallFont=TTF_OpenFont( path.string().c_str(), 24 );
    if (smallFont==nullptr) {
        throw std::runtime_error("Failed to load font "+path.string()+", error "+std::string(TTF_GetError())+"");
    }
    midFont=TTF_OpenFont( path.string().c_str(), 64 );
    if (midFont==nullptr) {
        throw std::runtime_error("Failed to load font "+path.string()+", error "+std::string(TTF_GetError())+"");
    }
    largeFont=TTF_OpenFont( path.string().c_str(), 128);
    if (largeFont==nullptr) {
        throw std::runtime_error("Failed to load font "+path.string()+", error "+std::string(TTF_GetError())+"");
    }

    loadingBackground=new texwrap(assetsPath()/"menuBackground.png",renderer);

    currentScene = new startMenu(renderer,smallFont,midFont,largeFont);
}

engine::~engine() {

    //Must be called before we destroy the renderer, which it hereby is
    delete currentScene;
    delete loadingBackground;

    if (renderer!=nullptr)
        SDL_DestroyRenderer(renderer);
    if (window!=nullptr)
        SDL_DestroyWindow(window);
    if (smallFont!=nullptr)
        TTF_CloseFont(smallFont);
    if (midFont!=nullptr)
        TTF_CloseFont(midFont);
    if (largeFont!=nullptr)
        TTF_CloseFont(largeFont);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void engine::run() {
    bool quit = false;

    inputData currentInput;

    uint32_t pmillis=SDL_GetTicks();

    while (!quit) {
        unsigned int millis = SDL_GetTicks();



        scene::openSceneCommand newSceneCommand=scene::MENU;
        std::string newSceneArguments;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_WINDOWEVENT) {
                switch( event.window.event ) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        windowWidthPx = event.window.data1;
                        windowHeightPx = event.window.data2;
                        break;
                    default:
                        break;
                }
            }
            if (event.type == SDL_MOUSEWHEEL) {
                currentInput.scroll=event.wheel.y;
            }
            if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState( &currentInput.mouseXPx, &currentInput.mouseYPx );
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentInput.leftMouseDown=true;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentInput.rightMouseDown=true;
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentInput.leftMouseDown=false;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentInput.rightMouseDown=false;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LEFT) {
                    currentInput.leftPressed=true;
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    currentInput.rightPressed=true;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    currentInput.upPressed=true;
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    currentInput.downPressed=true;
                }
                if (event.key.keysym.sym == SDLK_z) {
                    currentInput.zoomInPressed=true;
                }
                if (event.key.keysym.sym == SDLK_x) {
                    currentInput.zoomOutPressed=true;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    currentInput.sPressed=true;
                }
                if (event.key.keysym.sym == SDLK_v) {
                    currentInput.vPressed=true;
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    currentInput.enterPressed=true;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    currentInput.escapePressed=true;
                }
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_LEFT) {
                    currentInput.leftPressed=false;
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    currentInput.rightPressed=false;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    currentInput.upPressed=false;
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    currentInput.downPressed=false;
                }
                if (event.key.keysym.sym == SDLK_z) {
                    currentInput.zoomInPressed=false;
                }
                if (event.key.keysym.sym == SDLK_x) {
                    currentInput.zoomOutPressed=false;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    currentInput.sPressed=false;
                }
                if (event.key.keysym.sym == SDLK_v) {
                    currentInput.vPressed=false;
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    currentInput.enterPressed=false;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    currentInput.escapePressed=false;
                }
            }
        }
        SDL_Keymod modState = SDL_GetModState();
        if (modState & KMOD_SHIFT)
            currentInput.shiftPressed=true;
        else
            currentInput.shiftPressed=false;

        if (modState & KMOD_CTRL)
            currentInput.ctrlPressed=true;
        else
            currentInput.ctrlPressed=false;

        currentScene->update(renderer,*loadingBackground,windowWidthPx,windowHeightPx,currentInput,millis,pmillis,smallFont,midFont,largeFont);

        //Black background, shouldn't be seen but won't hurt
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear( renderer );

        currentScene->render(renderer,*loadingBackground,windowWidthPx,windowHeightPx,currentInput,millis,pmillis);
        //theScenario->render(renderer,mouseXPos,mouseYPos,shiftDown,millis);
        SDL_RenderPresent( renderer );


        if (currentScene->shouldOpenNewScene(newSceneCommand,newSceneArguments)) {
            if (newSceneCommand==scene::QUIT) {
                break;
            }
            if (newSceneCommand==scene::NEW_GAME){
                delete currentScene;
                currentScene = new game(renderer,windowWidthPx,windowHeightPx, *loadingBackground,newSceneArguments,smallFont);
            }
        }

        pmillis=millis;

        currentInput.prevLeftMouseDown=currentInput.leftMouseDown;
        currentInput.prevRightMouseDown=currentInput.rightMouseDown;
        currentInput.prevEnterPressed=currentInput.enterPressed;
        currentInput.prevEscapePressed=currentInput.escapePressed;
        currentInput.prevSPressed=currentInput.sPressed;
        currentInput.prevVPressed=currentInput.vPressed;
    }

}

