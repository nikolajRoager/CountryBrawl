//
// Created by nikolaj on 12/26/25.
//

#include "game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdPartyLibraryInclude/stb_image.h"

#include <future>
#include <iostream>
#include <SDL2/SDL_image.h>

#include "getAssets.h"
#include "mapData.h"

game::game(SDL_Renderer *renderer,int windowWidthPx, int windowHeightPx,const texwrap &loadingBackground,  const std::string& playerCountry, TTF_Font *smallFont) {
    std::cout<<"Loading new game"<<std::endl;
    //First, set up loading of everything we will be loading asynchronously
    std::vector<std::future<tile>> futureTiles;

    //A counter for how much stuff has been loaded
    std::atomic<int> processedAssets=0;

    int zoomLvl=0;
    //Set up functions for asynchronously loading pngs
    for (int n = 1; n <= gridWidth; n*=2) {
        for (int x = 0; x < gridWidth/n; x++)
            for (int y = 0; y < gridWidth/n; y++) {
                futureTiles.push_back( std::async(std::launch::async, [=, &processedAssets] {
                    try {
                        std::string fileName = "tile_"+std::to_string(zoomLvl)+"_"+std::to_string(y)+"_"+std::to_string(x)+".png";
                        tile t =tile(fileName,tileSize*x*n, tileSize*y*n, tileSize*n, tileSize*n,1.0/(2*n) ,n==1 ? std::pow(2.0,maxScaleExponent): 1.0/n );
                        ++processedAssets;
                        return t;
                    }
                    catch( std::exception& e ) {
                        ++processedAssets;//It was certainly processed, even if the outcome isn't what we wanted
                        throw;
                    }
                }));
            }
        zoomLvl++;
    }
    int totalAssets = futureTiles.size();


    //Two bespoke futures for loading additional map data (individually slower than the tiles)
    std::future<mapData> futureMovementPenalties=std::async(std::launch::async,[=,&processedAssets] {
        try {
            mapData movementPenalties("movementPenalties",tileSize*gridWidth,tileSize*gridWidth);
            ++processedAssets;
            return movementPenalties;
        }
        catch( std::exception& e ) {
            ++processedAssets;
            throw;
        }
    });
    totalAssets++;
    std::future<mapData> futureWatermap=std::async(std::launch::async,[=,&processedAssets] {
        try {
            mapData watermap("watermap",tileSize*gridWidth,tileSize*gridWidth);
            ++processedAssets;
            return watermap;
        }
        catch( std::exception& e ) {
            ++processedAssets;
            throw;
        }
    });
    totalAssets++;



    //Then load a few of the things which can't/won't be loaded asynchronously



    //Then display a loading bar
    //Display a loading bar
    int previousProcessedAssets = processedAssets;
    texwrap processedAssetsText(std::to_string(processedAssets)+"/"+std::to_string(totalAssets),renderer,smallFont);
    while (processedAssets < totalAssets) {

        //Respond to window resize events;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
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
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear( renderer );

        SDL_Rect loadingBarRect = { 0, windowHeightPx/2, (windowWidthPx*processedAssets)/totalAssets,  windowHeightPx/4};
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

        SDL_RenderFillRect( renderer, &loadingBarRect );

        if (processedAssets!=previousProcessedAssets) {
            previousProcessedAssets = processedAssets;
            processedAssetsText=texwrap (std::to_string(processedAssets)+"/"+std::to_string(totalAssets),renderer,smallFont);
        }
        processedAssetsText.render(windowWidthPx/2,windowHeightPx/2,renderer);

        SDL_RenderPresent( renderer );
    }

    //Then collect everything which has been loaded
    std::cout<<"Collecting tiles "<<processedAssets<<"/"<<totalAssets<<std::endl;

    tiles.reserve(futureTiles.size());
    //Collect the loaded tiles
    for (auto& future : futureTiles)
        try {
            tiles.push_back(future.get());
        }
        catch (std::exception& e) {
            std::cout <<"Loading failed with tile loading exception "<< e.what() << std::endl;
            //Re-package the exception so it is clear where it comes from
            throw std::runtime_error("Loading tiles failed with exception "+std::string(e.what()));
        }


    std::cout<<"Collecting mapdata"<<std::endl;
    try {
        std::cout<<"Collecting movement penalties"<<std::endl;
        movementPenalties=futureMovementPenalties.get();
        std::cout<<"Collecting watermap"<<std::endl;
        watermap = futureWatermap.get();
        std::cout<<"Collecting done"<<std::endl;
    }
    catch (std::exception& e) {
        //Re-package the exception so it is clear where it comes from
        throw std::runtime_error("Loading mapdata failed with exception "+std::string(e.what()));
    }
    std::cout<<"Created successfully"<<std::endl;


    //TEMP, TODO REMOVE, this is an example of how to create a texture without IMG_load

    fs::path path = assetsPath()/"menuBackground.png";
    int w, h, c;
    unsigned char* data = stbi_load(
        path.string().c_str(),
        &w, &h, &c,
        STBI_rgb_alpha
    );

    if (!data) {
        throw std::runtime_error(stbi_failure_reason());
    }
    std::cout<<"Loaded "<<w<<" "<<h<<" "<<c<<std::endl;

    width = w;
    height = h;

    tex=nullptr;

    std::vector<unsigned char> png;
    png.resize(width*height*4);
    memcpy(png.data(),data,width*height*4);
    stbi_image_free(data);

    auto surface = SDL_CreateRGBSurfaceWithFormatFrom(
            (void*)png.data(),
            width,
            height,
            32,
            width * 4,
            SDL_PIXELFORMAT_RGBA32
        );
    if (surface == nullptr) {
            throw std::runtime_error("Unable to load image: " + std::string(SDL_GetError()));
        }


    width = surface->w;
    height = surface->h;

    tex= SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (tex == nullptr) {
        throw std::runtime_error("Unable to create texture: " + std::string(SDL_GetError()));
    }


}

void game::render(SDL_Renderer *renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int pmillis) const {

    double scale=1.0;
    int x=0;
    int y=0;
    int frames=1;
    int frame=0;
    if (tex!=nullptr)
    {
        //Set rendering space and render to screen
        int w = (width)/frames;

        SDL_Rect renderQuad = { static_cast<int>(x), static_cast<int>(y), w, height};

        renderQuad.w *= scale;
        renderQuad.h *= scale;

        SDL_Rect srect = { w*(int)frame, 0, w, height };



        //Render to screen
        SDL_SetTextureColorMod(tex, 255,255,255);
        SDL_SetTextureAlphaMod(tex,255);
        SDL_Point centerPoint = {0, 0};
        SDL_RenderCopyEx( renderer, tex, &srect, &renderQuad ,0,&centerPoint ,SDL_FLIP_NONE);
    }
}

void game::update(SDL_Renderer *renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int dmillis, TTF_Font *smallFont, TTF_Font *midFont, TTF_Font *largeFont) {

}

bool game::shouldOpenNewScene(openSceneCommand &command, std::string &arguments) const {
    return false;
}

game::~game() {
    if (tex!=nullptr) SDL_DestroyTexture(tex);
}