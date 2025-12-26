//
// Created by nikolaj on 12/26/25.
//

#ifndef COUNTRYBRAWL_GAME_H
#define COUNTRYBRAWL_GAME_H
#include <vector>

#include "mapData.h"
#include "scene.h"
#include "tile.h"


class game : public scene{
public:
    game(SDL_Renderer* renderer,int windowWidthPx, int windowHeightPx, const texwrap &loadingBackground,  const std::string& playerCountry, TTF_Font* smallFont);
    ~game() override;

    void render(SDL_Renderer* renderer, const texwrap& loadingBackground,int screenWidth, int screenHeight,const inputData& userInputs, unsigned int millis, unsigned int pmillis) const override;
    void update(SDL_Renderer* renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight,const inputData& userInputs,  unsigned int millis, unsigned int dmillis,TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont) override;
    bool shouldOpenNewScene(openSceneCommand& command, std::string& arguments) const override;

private:
    //Each tile is 512x512 pixels
    int tileSize = 512;
    //My true grid width is 32 For a total of 16384x16384 pixels
    int gridWidth=32;
    double scaleExponent=0.0;
    double maxScaleExponent=2.0;
    double minScaleExponent=-6.0;//Must be so gridWidth=(2^(-minScaleExponent-1))

    std::vector<tile> tiles;

    mapData movementPenalties;
    mapData watermap;


    SDL_Texture* tex;
    int width, height;
};


#endif //COUNTRYBRAWL_GAME_H