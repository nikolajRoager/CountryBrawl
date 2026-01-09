//
// Created by nikolaj on 12/24/25.
//

#include "startMenu.h"

#include <algorithm>
#include <cmath>

#include "getAssets.h"

startMenu::startMenu(SDL_Renderer* renderer, TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont) : gameTitle("Country Brawl",renderer,largeFont), subTitle("Battle for Europe",renderer,midFont), newGameItem(renderer,50,300,midFont,"New Game"), settingsItem(renderer,50,380,midFont,"Settings"), quitGameItem(renderer,50,460,midFont,"Quit"), startNewGameItem(renderer,50,300,midFont,"Start Game"),goBackFromNewGameItem(renderer,50,380,midFont,"Back"), selectCountry("Select Country",renderer,midFont),
selectCountryLeft(renderer,900,200, assetsPath()/"ui"/"goLeft.png"),selectCountryRight(renderer,1030,200, assetsPath()/"ui"/"goRight.png"),selectedCountryName("null",renderer,midFont), ballInWater(assetsPath()/"countryballAccessories"/"ballInWater.png",renderer), selectedCountryDescription("null",renderer,smallFont),angryBall(assetsPath()/"countryballAccessories"/"angry.png",renderer),
happyBall(assetsPath()/"countryballAccessories"/"happy.png",renderer), veryEasyDifficultyText("Very Easy",renderer,midFont), easyDifficultyText("Easy",renderer,midFont), mediumDifficultyText("Medium",renderer,midFont),hardDifficultyText("Hard",renderer,midFont),veryHardDifficultyText("Very Hard",renderer,midFont),impossibleDifficultyText("Impossible",renderer,midFont), startingCitiesText("Cities: 0",renderer,midFont),
deadBall(assetsPath()/"countryballAccessories"/"dead.png",renderer), setMusicVolumeText("Music Volume",renderer,midFont),musicOffTexture(assetsPath()/"ui"/"menuMusicOff.png",renderer),musicFullTexture(assetsPath()/"ui"/"menuMusic.png",renderer)
{
    //Load all country paths first so we put them in alphabetic order
    {

        for (const auto& entry : fs::directory_iterator(fs::path("assets")/"countryballAccessories"/"guns")) {
            if (entry.path().extension()==".png") {
                guns.emplace(entry.path().filename().stem().string(),texwrap(entry.path(),renderer));
            }
        }

        std::vector<fs::directory_entry> countryPaths;
        for (const auto& entry : fs::directory_iterator(fs::path("assets")/"countryballs")) {
            if (entry.is_directory())
                countryPaths.push_back(entry);
        }

        std::sort(countryPaths.begin(), countryPaths.end(),
                  [](const fs::directory_entry& a,
                     const fs::directory_entry& b)
                  {
                      return a.path().filename() < b.path().filename();
                  });

        for (int i = 0; i < countryPaths.size(); i++)
        {
            const auto& entry = countryPaths[i];
            fs::path countryPath = entry.path();
            countries.emplace_back(i,countryPath,ballInWater,angryBall,happyBall,deadBall,guns,renderer,smallFont,midFont);
        }

        selectedCountryName=texwrap(countries[selectedCountry].getName(),renderer,midFont);
        selectedCountryDescription=texwrap(countries[selectedCountry].getDescription(),renderer,smallFont,600);
        startingCitiesText=texwrap("Cities: "+std::to_string(countries[selectedCountry].getStartingCities()),renderer,smallFont);

        bonusesText.clear();
        malusesText.clear();
        for (const std::string& str : countries[selectedCountry].getBonuses()) {
            bonusesText.emplace_back("Bonus: "+str,renderer,smallFont);
        }
        for (const std::string& str : countries[selectedCountry].getMaluses()) {
            malusesText.emplace_back("Malus: "+str,renderer,smallFont);
        }

    }

    newSceneCommand=MENU;
    newSceneArguments="";


    backgroundScale=1.0;

    musicVolumeBarWidth = 512;
    musicVolumeBarHeight = 64;
}

startMenu::~startMenu() = default;

bool startMenu::shouldOpenNewScene(openSceneCommand &command, std::string &arguments) const {
    if (newSceneCommand==MENU)
        return false;
    else {
        command=newSceneCommand;
        arguments=newSceneArguments;
        return true;
    }
}

void startMenu::update(SDL_Renderer* renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int dmillis,TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont, std::default_random_engine& generator, musicManager& muse) {

    backgroundScale = std::max(screenWidth/double(loadingBackground.getWidth()),screenHeight /double(loadingBackground.getHeight()));

    if (currentState ==MAIN) {
        if (userInputs.leftMouseDown && !userInputs.prevLeftMouseDown) {
            if (newGameItem.isHovered(userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale)) {
                currentState = SELECT_NEW_COUNTRY;
            }
            else if (quitGameItem.isHovered(userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale)) {
                newSceneCommand=QUIT;
            }
            else if (settingsItem.isHovered(userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale)) {
                currentState=SETTINGS;
            }
        }
    }
    else if (currentState==SETTINGS) {
        if (userInputs.leftMouseDown && !userInputs.prevLeftMouseDown) {
            if (goBackFromNewGameItem.isHovered(userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale)) {
                currentState = MAIN;
            }
        }
        if (userInputs.leftMouseDown){
            //Check if we clicked the bar which controls music volume
            int middleScreenX = screenWidth / 2;
            int middleScreenY = screenHeight / 2;
            int musicVolumeBarX0 = middleScreenX-backgroundScale*musicVolumeBarWidth/2;
            int musicVolumeBarY0 = middleScreenY;
            int newVolume=-1;
            if (userInputs.mouseYPx>musicVolumeBarY0 && userInputs.mouseYPx<musicVolumeBarY0+backgroundScale*musicVolumeBarHeight) {
                if (userInputs.mouseXPx>musicVolumeBarX0 && userInputs.mouseXPx<musicVolumeBarX0+backgroundScale*musicVolumeBarWidth) {
                    newVolume= (userInputs.mouseXPx-musicVolumeBarX0)/(backgroundScale*musicVolumeBarWidth/SDL_MIX_MAXVOLUME);
                    }
                else if (userInputs.mouseXPx<musicVolumeBarX0 && userInputs.mouseXPx>musicVolumeBarX0-backgroundScale*musicOffTexture.getWidth()) {
                    newVolume=0;
                }
                else if (userInputs.mouseXPx<musicVolumeBarX0+backgroundScale*musicVolumeBarWidth+backgroundScale*musicOffTexture.getWidth() && userInputs.mouseXPx>musicVolumeBarX0+backgroundScale*musicVolumeBarWidth) {
                    newVolume=SDL_MIX_MAXVOLUME;
                }
            }
            if (newVolume!=-1)
                muse.setMusicVolume(newVolume);
        }
    }
    else if (currentState ==SELECT_NEW_COUNTRY) {
        if (userInputs.leftMouseDown && !userInputs.prevLeftMouseDown) {
            if (goBackFromNewGameItem.isHovered(userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale)) {
                currentState = MAIN;
            }
            else if (startNewGameItem.isHovered(userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale)) {
                newSceneCommand=NEW_GAME;
                newSceneArguments=countries[selectedCountry].getName();
            }
            else if (selectCountryLeft.isHovered(userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale)) {
                selectedCountry=selectedCountry<=0?countries.size()-1:selectedCountry-1;
                //Change all textures and menus associated with the selected country
                selectedCountryName=texwrap(countries[selectedCountry].getName(),renderer,midFont);
                selectedCountryDescription=texwrap(countries[selectedCountry].getDescription(),renderer,smallFont,600);
                startingCitiesText=texwrap("Cities: "+std::to_string(countries[selectedCountry].getStartingCities()),renderer,smallFont);

                bonusesText.clear();
                malusesText.clear();
                for (const std::string& str : countries[selectedCountry].getBonuses()) {
                    bonusesText.emplace_back("Bonus: "+str,renderer,smallFont);
                }
                for (const std::string& str : countries[selectedCountry].getMaluses()) {
                    malusesText.emplace_back("Malus: "+str,renderer,smallFont);
                }
            }
            else if (selectCountryRight.isHovered(userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale)) {
                selectedCountry=(selectedCountry+1 )%countries.size();
                //Change all textures and menus associated with the selected country
                selectedCountryName=texwrap(countries[selectedCountry].getName(),renderer,midFont);
                selectedCountryDescription=texwrap(countries[selectedCountry].getDescription(),renderer,smallFont,600);
                startingCitiesText=texwrap("Cities: "+std::to_string(countries[selectedCountry].getStartingCities()),renderer,smallFont);

                bonusesText.clear();
                malusesText.clear();
                for (const std::string& str : countries[selectedCountry].getBonuses()) {
                    bonusesText.emplace_back("Bonus: "+str,renderer,smallFont);
                }
                for (const std::string& str : countries[selectedCountry].getMaluses()) {
                    malusesText.emplace_back("Malus: "+str,renderer,smallFont);
                }
            }
        }
    }
}

void startMenu::render(SDL_Renderer *renderer, const texwrap &loadingBackground,int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int pmillis, musicManager& muse) const {
    //We use the loading background as menu background
    //Set the scale so it covers the screen
    //We will use this scale on every menu item
    int tint =  currentState ==MAIN? 255 : 125;
    loadingBackground.render(0,0,tint,tint,tint,renderer,backgroundScale);

    gameTitle.render(0,0,255,128,64,renderer,backgroundScale);
    subTitle.render(0,gameTitle.getHeight()*backgroundScale,255,255,255,renderer,backgroundScale);

    if (currentState ==MAIN) {
        newGameItem.render(renderer,userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale);
        settingsItem.render(renderer,userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale);
        quitGameItem.render(renderer,userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale);
    }
    else if (currentState == SETTINGS) {
        goBackFromNewGameItem.render(renderer,userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale);


        int middleScreenX = screenWidth / 2;
        int middleScreenY = screenHeight / 2;
        setMusicVolumeText.render(middleScreenX,middleScreenY,renderer,backgroundScale,true,true);
        int musicVolumeBarX0 = middleScreenX-(int)(backgroundScale*musicVolumeBarWidth/2);
        int musicVolumeBarY0 = middleScreenY;
        SDL_Rect musicVolumeBarQuad{musicVolumeBarX0 ,musicVolumeBarY0,(int)(backgroundScale*musicVolumeBarWidth),(int)(backgroundScale*musicVolumeBarHeight)};
        SDL_SetRenderDrawColor(renderer,128,128,128,255);
        SDL_RenderFillRect(renderer,&musicVolumeBarQuad);
        int volume = muse.getMusicVolume();
        for (int i = 0; i < volume; ++i) {
            //Draw green quads at 3 px wide, for each volume level
            SDL_Rect quad{(int)(musicVolumeBarX0+backgroundScale*i*musicVolumeBarWidth/SDL_MIX_MAXVOLUME),musicVolumeBarY0, std::max(1,(int)(backgroundScale*musicVolumeBarWidth/SDL_MIX_MAXVOLUME-1)),(int)(backgroundScale*musicVolumeBarHeight)};
            SDL_SetRenderDrawColor(renderer,128,255,128,255);
            SDL_RenderFillRect(renderer,&quad);
        }
        musicOffTexture.render(musicVolumeBarX0-backgroundScale*musicOffTexture.getWidth(),musicVolumeBarY0,renderer,backgroundScale);
        musicFullTexture.render(musicVolumeBarX0+backgroundScale*musicVolumeBarWidth,musicVolumeBarY0,renderer,backgroundScale);
    }
    else if (currentState ==SELECT_NEW_COUNTRY) {
        startNewGameItem.render(renderer,userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale);
        goBackFromNewGameItem.render(renderer,userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale);
        selectCountryLeft.render(renderer,userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale);
        int countryX = (965+16)*backgroundScale;
        int countryY = 260*backgroundScale;
        int dx = userInputs.mouseXPx-countryX;
        int dy = userInputs.mouseYPx-countryY;
        double angle = atan2(dy,dx);
        countries[selectedCountry].display(countryX,countryY,false,country::HAPPY,backgroundScale,renderer,userInputs.mouseXPx>countryX,userInputs.mouseXPx>countryX ?angle : angle+M_PI);
        selectCountryRight.render(renderer,userInputs.mouseXPx,userInputs.mouseYPx,backgroundScale);
        selectCountry.render(965*backgroundScale,120*backgroundScale,renderer,backgroundScale,true);
        selectedCountryName.render(965*backgroundScale,294*backgroundScale,renderer,backgroundScale,true);
        startingCitiesText.render(965*backgroundScale,380*backgroundScale,renderer,backgroundScale,true);
        selectedCountryDescription.render(965*backgroundScale,420*backgroundScale,renderer,backgroundScale,true);

        switch (countries[selectedCountry].getDifficulty()) {
            case country::VERY_EASY:
                veryEasyDifficultyText.render(965*backgroundScale,(460+selectedCountryDescription.getHeight())*backgroundScale,0,255,0,renderer,backgroundScale,true);
                break;
            case country::EASY:
                easyDifficultyText.render(965*backgroundScale,(460+selectedCountryDescription.getHeight())*backgroundScale,128,255,0,renderer,backgroundScale,true);
                break;
            case country::MEDIUM:
                mediumDifficultyText.render(965*backgroundScale,(460+selectedCountryDescription.getHeight())*backgroundScale,255,255,0,renderer,backgroundScale,true);
                break;
            case country::HARD:
                hardDifficultyText.render(965*backgroundScale,(460+selectedCountryDescription.getHeight())*backgroundScale,255,125,0,renderer,backgroundScale,true);
                break;
            case country::VERY_HARD:
                veryHardDifficultyText.render(965*backgroundScale,(460+selectedCountryDescription.getHeight())*backgroundScale,255,0,0,renderer,backgroundScale,true);
                break;
            case country::IMPOSSIBLE:
                impossibleDifficultyText.render(965*backgroundScale,(460+selectedCountryDescription.getHeight())*backgroundScale,128,128,128,renderer,backgroundScale,true);
                break;
        }

        int lastY = 540+selectedCountryDescription.getHeight();
        for (const auto& bonus : bonusesText) {
            bonus.render(700*backgroundScale,lastY*backgroundScale,0,255,0,renderer,backgroundScale);
            lastY += bonus.getHeight();
        }
        for (const auto& malus: malusesText) {
            malus.render(700*backgroundScale,lastY*backgroundScale,255,255,0,renderer,backgroundScale);
            lastY += malus.getHeight();
        }
    }
}
