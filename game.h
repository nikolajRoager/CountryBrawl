//
// Created by nikolaj on 12/26/25.
//

#ifndef COUNTRYBRAWL_GAME_H
#define COUNTRYBRAWL_GAME_H
#include <vector>

#include "city.h"
#include "country.h"
#include "countryball.h"
#include "mapData.h"
#include "scene.h"
#include "tile.h"


class game : public scene{
public:
    game(SDL_Renderer* renderer,int windowWidthPx, int windowHeightPx, const texwrap &loadingBackground,  const std::string& playerCountry, TTF_Font* smallFont);
    ~game() override;

    void render(SDL_Renderer* renderer, const texwrap& loadingBackground,int screenWidth, int screenHeight,const inputData& userInputs, unsigned int millis, unsigned int pmillis) const override;
    void update(SDL_Renderer* renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight,const inputData& userInputs,  unsigned int millis, unsigned int pmillis,TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont) override;
    bool shouldOpenNewScene(openSceneCommand& command, std::string& arguments) const override;

private:
    //Each tile is 512x512 pixels
    int tileSize = 512;
    //My true grid width is 32 For a total of 16384x16384 pixels
    int gridWidth=32;
    double scaleExponent=0.0;
    double maxScaleExponent=2.0;
    double minScaleExponent=-6.0;//Must be so gridWidth=(2^(-minScaleExponent-1))

    //TODO, for mod ability, this should be loaded from a file
    //For calculating longitude and latitude
    int mapHeight = 16384;
    int mapWidth = mapHeight;

    //TODO, this too should be loaded from a file
    //This roughly coincides with Europe, my favourite continent
    double long0 = -13;
    double long1 = +44;
    double lat0 =-75;
    double lat1 =lat0+long1-long0;


    double screenMinX=0, screenMinY=0;
    double scale=1.0;



    //Accessories for countryballs
    texwrap ballInWater;
    texwrap angryBall;
    texwrap happyBall;
    std::map<std::string,texwrap> guns;


    //Accessories for city rendering
    texwrap cityTexture;

    //The list of countries the player can choose between
    std::vector<country> countries;
    int playerCountryId= 0;

    std::vector<city> cities;


    std::vector<std::unique_ptr<tile>> tiles;

    mapData movementPenalties;
    mapData watermap;


    std::vector<std::shared_ptr<countryball>> soldiers;
};


#endif //COUNTRYBRAWL_GAME_H