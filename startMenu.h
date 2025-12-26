//
// Created by nikolaj on 12/24/25.
//

#ifndef COUNTRYBRAWL_STARTMENU_H
#define COUNTRYBRAWL_STARTMENU_H
#include <vector>

#include "menuItem.h"
#include "scene.h"
#include "country.h"


class startMenu : public scene {
public:
    startMenu(SDL_Renderer* renderer, TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont);
    ~startMenu() override;

    void render(SDL_Renderer* renderer, const texwrap& loadingBackground,int screenWidth, int screenHeight,const inputData& userInputs, unsigned int millis, unsigned int pmillis) const override;
    void update(SDL_Renderer* renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight,const inputData& userInputs,  unsigned int millis, unsigned int dmillis,TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont) override;
    bool shouldOpenNewScene(openSceneCommand& command, std::string& arguments) const override;
private:
    enum menuState {MAIN,SELECT_NEW_COUNTRY,SETTINGS};
    menuState currentState = MAIN;

    //Accessories for countryballs, most won't be used in the menu, but they need to be loaded
    texwrap ballInWater;
    texwrap angryBall;
    texwrap happyBall;

    //The list of countries the player can choose between
    std::vector<country> countries;
    int selectedCountry = 0;

    //Main menu items
    menuItem newGameItem;
    menuItem quitGameItem;


    //start new game with new country items
    menuItem startNewGameItem;
    menuItem goBackFromNewGameItem;

    menuItem selectCountryLeft;
    menuItem selectCountryRight;

    texwrap gameTitle;
    texwrap subTitle;
    texwrap selectCountry;
    texwrap selectedCountryName;

    texwrap veryEasyDifficultyText;
    texwrap easyDifficultyText;
    texwrap mediumDifficultyText;
    texwrap hardDifficultyText;
    texwrap veryHardDifficultyText;
    texwrap impossibleDifficultyText;
    texwrap startingCitiesText;
    std::vector<texwrap> bonusesText;
    std::vector<texwrap> malusesText;

    texwrap selectedCountryDescription;

    double backgroundScale;

    openSceneCommand newSceneCommand;
    std::string newSceneArguments;
};


#endif //COUNTRYBRAWL_STARTMENU_H