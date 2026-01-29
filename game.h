//
// Created by nikolaj on 12/26/25.
//

#ifndef COUNTRYBRAWL_GAME_H
#define COUNTRYBRAWL_GAME_H
#include <deque>
#include <random>
#include <vector>

#include "cargoTicket.h"
#include "city.h"
#include "country.h"
#include "countryball.h"
#include "diplomacyManager.h"
#include "explosion.h"
#include "lingeringShot.h"
#include "mapData.h"
#include "missile.h"
#include "numberRenderer.h"
#include "scene.h"
#include "soundWrap.h"
#include "supplyHub.h"
#include "ticket.h"
#include "tile.h"
#include "uiArmyCapCounter.h"
#include "uiBottomBar.h"
#include "uiButton.h"
#include "uiCalendar.h"
#include "uiCityCounter.h"
#include "uiExpandableMenu.h"
#include "uiFundsTracker.h"
#include "uiTopBar.h"


class game : public scene{
public:
    game(SDL_Renderer* renderer,int windowWidthPx, int windowHeightPx, const texwrap &loadingBackground,  const std::string& playerCountry, TTF_Font* smallFont, TTF_Font *midFont);
    ~game() override;

    void render(SDL_Renderer* renderer, const texwrap& loadingBackground,int screenWidth, int screenHeight,const inputData& userInputs, unsigned int millis, unsigned int pmillis, musicManager& muse) const override;
    void update(SDL_Renderer* renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight,const inputData& userInputs,  unsigned int millis, unsigned int pmillis,TTF_Font* smallFont, TTF_Font* midFont, TTF_Font* largeFont, std::default_random_engine& generator, musicManager& muse) override;
    bool shouldOpenNewScene(openSceneCommand& command, std::string& arguments) const override;

    void togglePause() {paused = !paused;};

private:
    void balanceFrontLinesWar(int targetCountry);
    void balanceFrontLinesPeace(int targetCountry);
    void recalculateNeighbours();

    [[nodiscard]] std::map<int,int> getReinforcementPaths(const std::map<int,int>& citiesWithRequestedSoldiers, int targetCountry) const;

    std::map<std::string,std::string> eventMessages;
    std::vector<std::string> tensionDownEvents;
    std::vector<std::string> tensionUpEvents;
    std::vector<std::string> accidentalWarEvents;


    //millis when we last printed FPS
    unsigned int previousFPSprintMillis;
    //Frames since we last printed FPS
    unsigned int framesSinceFPSprint;

    //True only on the first update, this flag tells us to pretend the game was paused while we loaded
    bool firstUpdate;

    bool paused;

    texwrap pausedText;

    //milliseconds since the start of the game, not incremented while pause or loading
    unsigned int gameRealTime;
    //Start of in-game time
    std::chrono::sys_time<std::chrono::milliseconds> gameEpoch;
    std::chrono::sys_time<std::chrono::milliseconds> previousGameTime;
    double timewarpFactor;

    UITopBar topBar;
    UIBottomBar bottomBar;
    //The calendar, which we share with the top-bar
    std::shared_ptr<uiCalendar> calendar;
    std::shared_ptr<uiCityCounter> cityCounter;
    std::shared_ptr<uiArmyCapCounter> armyCapCounter;
    std::shared_ptr<uiFundsTracker> fundsTracker;

    std::shared_ptr<uiExpandableMenu> autoMissileMenu;
    std::shared_ptr<uiExpandableMenu> autoRecruitMenu;
    std::shared_ptr<uiExpandableMenu> stanceMenu;
    std::shared_ptr<uiButton> autoBalanceButton;
    std::shared_ptr<uiButton> musicManagerButton;
    std::shared_ptr<uiButton> diplomacyButton;

    std::set<int> selectedCities;
    int primarySelectedCity;
    //City we currently are hovering over
    int hoveredCity;

    double boxSelectionX0;
    double boxSelectionY0;
    bool boxSelectionActive;

    //TODO, for mod ability, this should be loaded from a file
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
    texwrap deadBall;
    std::map<std::string,texwrap> guns;


    //Accessories for city rendering
    texwrap ruinTexture;
    texwrap cityTexture;
    texwrap factoryTexture;
    texwrap missileSiteTexture;
    texwrap missileOnSiteTexture;
    texwrap supplyHubTexture;
    texwrap airDefenceTexture;
    texwrap selectedCityTexture;
    texwrap arrowTexture;
    texwrap circleMarkerTexture;

    //Accessories for rendering city information box
    texwrap cityColonTexture;
    texwrap provinceColonTexture;
    texwrap coreColonTexture;
    texwrap ownerColonTexture;
    texwrap developmentColonTexture;
    texwrap developTexture;
    texwrap changeSpecializationTexture;
    texwrap specializationColonTexture;
    texwrap airDefenceColonTexture;
    texwrap yesTexture;
    texwrap noTexture;
    texwrap stockpileColonTexture;
    texwrap productionColonTexture;
    texwrap incomeColonTexture;
    texwrap euroTexture;
    texwrap armyCapColonTexture;
    texwrap factoryTextTexture;
    texwrap missileSiteTextTexture;
    texwrap noneTextTexture;
    texwrap developMouseOverText;
    texwrap developMaxMouseOverText;
    texwrap addAirDefenceMouseOverText;
    texwrap bulletsTexture;

    //Accessories for trains
    texwrap trainEnd;
    texwrap trainSegment;
    texwrap passengerShip;
    texwrap transportPlane;
    texwrap cargoTrain;
    texwrap cargoShip;

    //Accessories for messages
    texwrap receivedMessage;
    texwrap okText;
    texwrap yesText;
    texwrap noText;

    //Accessories for map modes
    texwrap missileMapModeText;
    texwrap supplyMapModeText;
    texwrap neighbourMapModeText;
    texwrap missileAimMarker;

    //Accessories for missiles
    texwrap missileInAir;
    texwrap SAMInAir;

    //Accessories for explosions
    texwrap bigExplosion;


    //Sound effects for fighting
    soundWrap shotSound;
    soundWrap explosionSound;

    //The list of countries the player can choose between
    std::vector<country> countries;
    //The country was there last frame
    std::vector<bool> countryExisted;

    //Unification events, saved as "name of union" list of countries involved
    std::vector<std::pair<std::string,std::vector<int>>> unificationEvents;

    int playerCountryId= 0;

    std::vector<city> cities;
    //Supply hubs, indexed by the city id they are associated with
    std::map<int,supplyHub> supplyHubs;

    std::list<ticket> tickets;
    std::list<std::shared_ptr<cargoTicket>> cargoTickets;

    //Missiles in the air
    std::list<missile> missiles;
    std::list<missile> SAMs;
    double missileExplosionRadius;
    double missileRange;
    double SAMRange;

    std::deque<explosion> explosions;

    std::vector<std::unique_ptr<tile>> tiles;
    numberRenderer numbererSmall;
    numberRenderer numbererMid;

    mapData movementPenalties;
    mapData watermap;


    std::vector<std::map<int,int>> frontlinePathByCountry;

    std::vector<std::shared_ptr<countryball>> soldiers;

    std::deque<lingeringShot> smallArmsShots;

    double msPerFrame;

    bool musicManagerOpen;
    bool diploMenuOpen;

    int diploNegotiatingWith;

    std::unique_ptr<diplomacyManager > diploManager;


    std::vector<int> selectedPath;
};


#endif //COUNTRYBRAWL_GAME_H