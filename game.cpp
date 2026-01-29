//
// Created by nikolaj on 12/26/25.
//

#include "game.h"

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <cmath>
#include <fstream>
#include <ranges>
#include <SDL2/SDL_image.h>

#include "getAssets.h"
#include "jsonClient.h"
#include "lingeringShot.h"
#include "mapData.h"
#include "threadPool.h"
#include "uiCalendar.h"


game::game(SDL_Renderer *renderer, int windowWidthPx, int windowHeightPx, const texwrap &loadingBackground,
           const std::string &playerCountry, TTF_Font *smallFont, TTF_Font *midFont) : ballInWater(
        assetsPath() / "countryballAccessories" / "ballInWater.png", renderer),
    happyBall(assetsPath() / "countryballAccessories" / "happy.png", renderer),
    angryBall(assetsPath() / "countryballAccessories" / "angry.png", renderer),
    deadBall(assetsPath() / "countryballAccessories" / "dead.png", renderer),
    ruinTexture(assetsPath() / "ruin.png", renderer),
    cityTexture(assetsPath() / "city.png", renderer),
    factoryTexture(assetsPath()/"factory.png",renderer),
    missileSiteTexture(assetsPath()/"missileSite.png",renderer),
    missileOnSiteTexture(assetsPath()/"missile.png",renderer),
    missileInAir(assetsPath()/"missileInAir.png",renderer),
    SAMInAir(assetsPath()/"SAMInAir.png",renderer),
    supplyHubTexture(assetsPath() / "supplyHub.png", renderer),
    airDefenceTexture(assetsPath() / "airDefence.png", renderer),
    selectedCityTexture(assetsPath() / "selectedCity.png", renderer),
    arrowTexture(assetsPath() / "arrow.png", renderer),
    circleMarkerTexture(assetsPath() / "circleMarker.png", renderer),
    trainEnd(assetsPath() / "trainEnd.png", renderer),
    trainSegment(assetsPath() / "trainSegment.png", renderer),
    cargoTrain(assetsPath() / "cargoTrain.png", renderer),
    cargoShip(assetsPath() / "cargoShip.png", renderer),
    passengerShip(assetsPath() / "passengerShip.png", renderer),
    transportPlane(assetsPath() / "transportPlane.png", renderer),
    numbererSmall(0, smallFont, renderer),
    numbererMid(0, midFont, renderer),
    pausedText("Paused", renderer, midFont),
    receivedMessage("Message received",renderer,midFont),
    okText("Ok",renderer,midFont),
    yesText("Yes",renderer,midFont),
    noText("No",renderer,midFont),

    cityColonTexture("City: ",renderer,smallFont),
    provinceColonTexture("Province: ",renderer,smallFont),
    coreColonTexture("Core: ",renderer,smallFont),
    ownerColonTexture("Owner: ",renderer,smallFont),
    developmentColonTexture("Development: ",renderer,smallFont),
    developTexture(assetsPath()/"ui"/"develop.png",renderer),
    changeSpecializationTexture(assetsPath()/"ui"/"changeSpecialization.png",renderer),
    specializationColonTexture("Specialization: ",renderer,smallFont),
    airDefenceColonTexture("Air defence: ",renderer,smallFont),
    yesTexture(assetsPath()/"ui"/"yes.png",renderer),
    noTexture(assetsPath()/"ui"/"no.png",renderer),
    stockpileColonTexture("Stockpile:",renderer,smallFont),
    factoryTextTexture("Factory ",renderer,smallFont),
    missileSiteTextTexture("Missile Site ",renderer,smallFont),
    noneTextTexture("None ",renderer,smallFont),
    incomeColonTexture("Income: ",renderer,smallFont),
    euroTexture("€",renderer,smallFont),
    armyCapColonTexture("Army Cap: ",renderer,smallFont),
    productionColonTexture("Production: ",renderer,smallFont),
    bulletsTexture(assetsPath()/"ui"/"bullets.png",renderer),

    missileMapModeText("Missile Map Mode",renderer,smallFont),
    supplyMapModeText("Supply Map Mode",renderer,smallFont),
    neighbourMapModeText("Neighbour Map Mode",renderer,smallFont),
    missileAimMarker(assetsPath()/"missileAim.png",renderer),

    bigExplosion(assetsPath()/"explosion.png",renderer),

    developMouseOverText("Develop, €",renderer,smallFont),
    developMaxMouseOverText("Max developed already",renderer,smallFont),
    addAirDefenceMouseOverText("Add air defence, €",renderer,smallFont),
    shotSound(assetsPath()/"sound"/"shot.wav"),
    explosionSound(assetsPath()/"sound"/"explosion.wav"),
    topBar(renderer),
    bottomBar(renderer)
{
    std::cout << "Loading new game" << std::endl;
    //First, set up loading of everything we will be loading asynchronously

    //A counter for how much stuff has been loaded
    std::atomic<int> processedAssets{0};
    std::atomic<bool> cancel{false};
    std::string exceptionMessage;

    int zoomLvl = 0;

    //Limited scope, to limit the lifetime of the thread pool
    {
        threadPool loadingPool(std::thread::hardware_concurrency());
        //Mutex used when writing to results
        std::mutex resultMutex;

        int totalAssets = 0;
        //Loops over all 1365 locations and zoom level combinations
        for (int n = 1; n <= gridWidth; n *= 2) {
            for (int x = 0; x < gridWidth / n; x++)
                for (int y = 0; y < gridWidth / n; y++) {
                    loadingPool.enqueue(
                        [&resultMutex,&processedAssets,zoomLvl,&cancel,&loadingPool,&exceptionMessage,y,x,n, this]() {
                            if (!cancel.load(std::memory_order_relaxed))
                                try {
                                    std::string fileName =
                                            "tile_" + std::to_string(zoomLvl) + "_" + std::to_string(y) + "_" +
                                            std::to_string(x) + ".png";
                                    auto t = std::make_unique<tile>(fileName, tileSize * x * n, tileSize * y * n,
                                                                    tileSize * n, tileSize * n, 1.0 / (2 * n),
                                                                    n == 1 ? std::pow(2.0, maxScaleExponent) : 1.0 / n);

                                    std::lock_guard<std::mutex> lock(resultMutex);
                                    tiles.push_back(std::move(t));
                                    processedAssets.fetch_add(1);
                                } catch (std::exception &e) {
                                    //The result mutex pulls double duty, and also locks the exception message, that is a sort of result ... I gues
                                    std::lock_guard<std::mutex> lock(resultMutex);
                                    //It was certainly processed, even if the outcome isn't what we wanted
                                    processedAssets.fetch_add(1);
                                    bool expected = false;
                                    if (cancel.compare_exchange_strong(expected, true)) {
                                        exceptionMessage = e.what();
                                        loadingPool.cancel(); // stop new tasks immediately
                                    }
                                }
                        });
                    ++totalAssets;
                }
            zoomLvl++;
        }


        loadingPool.enqueue([&resultMutex,&processedAssets,&cancel, this, &exceptionMessage, &loadingPool] {
            if (!cancel.load(std::memory_order_relaxed))
                try {
                    mapData _movementPenalties("movementPenalties", tileSize * gridWidth, tileSize * gridWidth);
                    std::lock_guard<std::mutex> lock(resultMutex);
                    processedAssets.fetch_add(1);
                    movementPenalties = std::move(_movementPenalties);
                } catch (std::exception &e) {
                    //The result mutex pulls double duty, and also locks the exception message, that is a sort of result ... I gues
                    std::lock_guard<std::mutex> lock(resultMutex);
                    //It was certainly processed, even if the outcome isn't what we wanted
                    processedAssets.fetch_add(1);
                    bool expected = false;
                    if (cancel.compare_exchange_strong(expected, true)) {
                        exceptionMessage = e.what();
                        loadingPool.cancel(); // stop new tasks immediately
                    }
                }
        });
        totalAssets++;
        loadingPool.enqueue([&resultMutex,&processedAssets, &cancel, this, &loadingPool, &exceptionMessage] {
            if (!cancel.load(std::memory_order_relaxed))
                try {
                    mapData _watermap("watermap", tileSize * gridWidth, tileSize * gridWidth);
                    std::lock_guard<std::mutex> lock(resultMutex);
                    processedAssets.fetch_add(1);
                    watermap = std::move(_watermap);
                } catch (std::exception &e) {
                    //The result mutex pulls double duty, and also locks the exception message, that is a sort of result ... I gues
                    std::lock_guard<std::mutex> lock(resultMutex);
                    //It was certainly processed, even if the outcome isn't what we wanted
                    processedAssets.fetch_add(1);
                    bool expected = false;
                    if (cancel.compare_exchange_strong(expected, true)) {
                        exceptionMessage = e.what();
                        loadingPool.cancel(); // stop new tasks immediately
                    }
                }
        });
        totalAssets++;


        //Then load a few of the things which can't/won't be loaded asynchronously
        //These are things which either: require the SDL renderer (not thread safe), or: where the loading order is important
        //From testing these things are basically instant
        for (const auto &entry: fs::directory_iterator(fs::path("assets") / "countryballAccessories" / "guns")) {
            if (entry.path().extension() == ".png") {
                guns.emplace(entry.path().filename().stem().string(), texwrap(entry.path(), renderer));
            }
        }


        std::vector<fs::directory_entry> countryPaths;
        for (const auto &entry: fs::directory_iterator(fs::path("assets") / "countryballs")) {
            if (entry.is_directory())
                countryPaths.push_back(entry);
        }

        std::sort(countryPaths.begin(), countryPaths.end(),
                  [](const fs::directory_entry &a,
                     const fs::directory_entry &b) {
                      return a.path().filename() < b.path().filename();
                  });

        //Default to the first country, alphabetically (Albania)
        playerCountryId = 0;
        countries.reserve(countryPaths.size());
        //TODO, we really SHOULD multithread this, i.e. finalize it later
        for (int i = 0; i < countryPaths.size(); i++) {
            const auto &entry = countryPaths[i];
            fs::path countryPath = entry.path();
            countries.emplace_back(i, countryPath, ballInWater, angryBall, happyBall, deadBall, guns, renderer,smallFont,midFont);
            if (countries[i].getName() == playerCountry) {
                playerCountryId = i;
            }
        }

        //Also load cities, this MUST happen after countries
        {
            jsonClient importer(assetsPath() / "startingMap" / "cities.json");
            try {
                importer.load(cities, countries, supplyHubs);
            } catch (std::exception &e) {
                throw std::runtime_error("Could not load cities, error: " + std::string(e.what()));
            }
        }


        for (const auto &entry: fs::directory_iterator(fs::path("assets") / "eventMessages")) {
            if (entry.is_regular_file()) {
                std::ifstream eventFile(entry.path());
                std::string text;
                std::string line;
                std::getline(eventFile, line);
                text+=line;
                while (std::getline(eventFile, line)) {
                    text+="\n"+line;
                }
                std::string name = entry.path().stem().filename().string();
                if (name.starts_with("tensionDown"))
                    tensionDownEvents.emplace_back(name);
                if (name.starts_with("tensionUp"))
                    tensionUpEvents.emplace_back(name);
                if (name.starts_with("accidentWar"))
                    accidentalWarEvents.emplace_back(name);
                eventMessages.emplace(name, text);
            }
        }

        for (const auto &entry: fs::directory_iterator(fs::path("assets") / "unions")) {
            if (entry.is_regular_file()) {
                std::string name = entry.path().stem().filename().string();
                std::vector<int> targetCountries;
                std::ifstream eventFile(entry.path());
                std::string line;
                while (std::getline(eventFile, line)) {
                    bool found=false;
                    for (int i = 0; i < countries.size(); i++) {
                        if (countries[i].getName() == line) {
                            targetCountries.push_back(i);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        throw std::runtime_error("Could not find country " + line+" from unification event "+name);
                    }
                }
                if (!eventMessages.contains(name)) {
                    throw std::runtime_error("Could not find event message for unification event " + name);
                }
                unificationEvents.emplace_back(name, targetCountries);
            }
        }


        //Then display a loading bar
        //Display a loading bar
        int previousProcessedAssets = processedAssets;
        texwrap processedAssetsText(std::to_string(processedAssets) + "/" + std::to_string(totalAssets), renderer,
                                    smallFont);
        while (processedAssets < totalAssets && !cancel.load()) {
            //Respond to window resize events;
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_WINDOWEVENT) {
                    switch (event.window.event) {
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
            SDL_RenderClear(renderer);
            loadingBackground.render(0, 0, renderer, 1);
            SDL_Rect loadingBarRect = {
                0, windowHeightPx / 2, (windowWidthPx * processedAssets) / totalAssets, windowHeightPx / 4
            };
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

            SDL_RenderFillRect(renderer, &loadingBarRect);

            if (processedAssets != previousProcessedAssets) {
                previousProcessedAssets = processedAssets;
                processedAssetsText = texwrap(std::to_string(processedAssets) + "/" + std::to_string(totalAssets),
                                              renderer, smallFont);
            }
            processedAssetsText.render(windowWidthPx * 0.5, windowHeightPx * 0.5, renderer);

            SDL_RenderPresent(renderer);
        }

        if (cancel.load()) {
            std::lock_guard<std::mutex> lock(resultMutex);
            throw std::runtime_error("The loading cancelled due to: " + exceptionMessage);
        }

        std::cout << "Finalizing" << std::endl;
        for (auto &tile: tiles) {
            tile->finalize();
        }

        diploManager=std::make_unique<diplomacyManager>(assetsPath()/"startingMap"/"tensions.csv",countries,renderer,smallFont,midFont);
    }

    //Then do the things which require the things we just loaded to exist
    double cameraCentreX = 0;
    double cameraCentreY = 0;
    int playerCities = 0;

    //Todo multithreading this might be an idea
    for (auto &city: cities) {
        city.updateFrontlinesAndNeighbourDistances(cities, watermap);
        city.generateNameTexture(smallFont, renderer);
        city.updateNeighbourhood(cities);
        if (city.getOwner() == playerCountryId) {
            cameraCentreX += city.getX();
            cameraCentreY += city.getY();
            playerCities++;
        }
    }
    if (playerCities>0) {
        cameraCentreX /= playerCities;
        cameraCentreY /= playerCities;
    }


    //Also load the soldiers
    {
        jsonClient importer(assetsPath() / "startingMap" / "startingArmies.json");
        try {
            importer.loadSoldiersAtGamestart(soldiers, cities, countries,*diploManager);
        } catch (std::exception &e) {
            throw std::runtime_error("Could not load cities, error: " + std::string(e.what()));
        }
    }


    //Centre the camera on the player country
    screenMinX = cameraCentreX * scale - windowWidthPx / 2.0;
    screenMinY = cameraCentreY * scale - windowHeightPx / 2.0;

    for (auto &hub: supplyHubs | std::views::values) {
        hub.recalculate(cities,countries,supplyHubs);
    }

    //Calculate which hubs can potentially be neighbours

    for (const auto& city : cities) {
        const auto& potentialHubs = city.getPotentialSupplyHubs();
        for (int i : potentialHubs) {
            for (int j : potentialHubs) {
                if (i!=j) {
                    supplyHubs.at(i).addSupplyHubNeighbour(j);
                    supplyHubs.at(j).addSupplyHubNeighbour(i);
                }
            }
        }
    }

    frontlinePathByCountry.resize(countries.size());

    std::cout << "Loaded " << countries.size() << " countries " << cities.size() << " cities and " << soldiers.size() <<
            " soldiers " << std::endl;


    //Count the number of core and occupied cities of each nation
    for (auto &country: countries) {
        country.setCoreCities(0);
        country.setOccupiedCities(0);
    }
    for (int i = 0; i < cities.size(); i++) {
        const auto &city = cities[i];
        int owner = city.getOwner();
        int core = city.getCore();
        if (owner == core)
            countries[city.getOwner()].incrementCoreCities();
        else
            countries[city.getOwner()].incrementOccupiedCities();
        countries[core].addCoreId(i);
    }

    //Set up ui elements
    //I think hardcoding it is fine
    calendar = std::make_shared<uiCalendar>(renderer, midFont, smallFont);
    topBar.addRightComponent(calendar);

    cityCounter = std::make_shared<uiCityCounter>(renderer, midFont, smallFont);
    topBar.addRightComponent(cityCounter);

    armyCapCounter = std::make_shared<uiArmyCapCounter>(renderer, midFont, smallFont, &countries[playerCountryId]);
    topBar.addRightComponent(armyCapCounter);

    fundsTracker = std::make_shared<uiFundsTracker>(renderer, midFont, smallFont);
    topBar.addRightComponent(fundsTracker);


    autoRecruitMenu = std::make_shared<uiExpandableMenu>(renderer, smallFont,std::vector<std::string>{"recruitInfantry","recruitArtillery","recruitmentOff"},"Automatic recruitment settings");
    bottomBar.addRightComponent(autoRecruitMenu);


    autoMissileMenu= std::make_shared<uiExpandableMenu>(renderer, smallFont,std::vector<std::string>{"autoMissileOff","autoMissile"},"Automatically fire missiles");
    bottomBar.addRightComponent(autoMissileMenu);

    autoBalanceButton = std::make_shared<uiButton>(renderer, smallFont,"autoBalanceButton","Auto balance Front-Lines");
    bottomBar.addRightComponent(autoBalanceButton);

    stanceMenu = std::make_shared<uiExpandableMenu>(renderer, smallFont,std::vector<std::string>{"defensiveStance","cautiousAdvance","aggressiveAdvance"},"Auto Attack Stance");
    bottomBar.addRightComponent(stanceMenu);

    musicManagerButton = std::make_shared<uiButton>(renderer, smallFont,"musicButton","Open music manager");
    bottomBar.addLeftComponent(musicManagerButton);

    diplomacyButton = std::make_shared<uiButton>(renderer, smallFont,"diplomacyButton","Open diplomacy menu");
    bottomBar.addLeftComponent(diplomacyButton);

    std::cout << "Created successfully" << std::endl;

    previousFPSprintMillis = SDL_GetTicks();
    framesSinceFPSprint = 0;
    firstUpdate = true;
    primarySelectedCity = -1; //Deselected
    selectedCities.clear();

    gameRealTime = 0;
    gameEpoch = std::chrono::sys_days{std::chrono::year{2026} / 1 / 1};

    boxSelectionX0 = 0;
    boxSelectionY0 = 0;
    boxSelectionActive = false;
    hoveredCity = -1; //None
    msPerFrame = 17;

    paused = true;
    musicManagerOpen = false;
    diploMenuOpen=false;
    diploNegotiatingWith=-1;

    //TODO, this should be loadable from a file
    timewarpFactor = 43200; //1 month per minute, roughly 1 day per 2 seconds

    previousGameTime = gameEpoch;
    countryExisted.resize(countries.size(),true);

    //The explosion size is taken from the animation
    missileExplosionRadius=missileAimMarker.getWidth()/2;
    missileRange=2048;
    SAMRange=512;
}

void game::render(SDL_Renderer *renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight,
                  const inputData &userInputs, unsigned int millis, unsigned int pmillis, musicManager& muse) const {

    double backgroundScale = std::max(screenWidth/double(loadingBackground.getWidth()),screenHeight /double(loadingBackground.getHeight()));

    for (const auto &tile: tiles) {
        tile->draw(static_cast<int>(screenMinX), static_cast<int>(screenMinY), scale, renderer);
    }


    bool missileMapMode=false;
    bool supplyMapMode=false;
    bool neighbourMapMode=false;

    if (userInputs.mPressed)
        missileMapMode = true;
    else if (userInputs.sPressed)
        supplyMapMode = true;
    else if (userInputs.nPressed)
        neighbourMapMode = true;


    for (int i = 0; i < cities.size(); i++) {
        const city &city = cities[i];
        city.display(cityTexture,factoryTexture,missileSiteTexture,missileOnSiteTexture, ruinTexture, selectedCityTexture, supplyHubTexture,airDefenceTexture,arrowTexture, selectedCities.contains(i), i == primarySelectedCity,
                     countries, cities, supplyHubs, screenMinX, screenMinY, screenWidth, screenHeight, scale, renderer, numbererSmall,supplyMapMode,neighbourMapMode,millis);
    }


    for (const auto &ticket: tickets) {
        ticket.display(cities, trainEnd, trainSegment, passengerShip,transportPlane, screenMinX, screenMinY, screenWidth, screenHeight,
                       scale, renderer, watermap);
    }

    for (const auto &ticket: cargoTickets) {
        ticket->display(cities, cargoTrain, cargoShip, screenMinX, screenMinY, screenWidth, screenHeight,
                       scale, renderer, watermap);
    }


    for (int i: selectedCities) {
        const city &city = cities[i];
        city.highlightNeighbour(arrowTexture, hoveredCity, cities, screenMinX, screenMinY, screenWidth, screenHeight,
                                scale, renderer, millis);
    }

    for (int i = 0; i + 1 < selectedPath.size(); i++) {
        cities[selectedPath[i]].highlightNeighbour(arrowTexture, selectedPath[i + 1], cities, screenMinX, screenMinY,
                                                   screenWidth, screenHeight, scale, renderer, millis);
    }

    for (const auto &shot: smallArmsShots) {
        shot.display(screenMinX, screenMinY, screenWidth, screenHeight, scale, renderer);
    }

    for (const auto &soldier: soldiers) {
        soldier->display(screenMinX, screenMinY, screenWidth, screenHeight, scale, renderer);
    }

    for (const auto &explosion : explosions) {
        explosion.display(screenMinX, screenMinY, screenWidth, screenHeight,scale,renderer);
    }

    for (const auto &missile : missiles) {
        missile.display(screenMinX, screenMinY, screenWidth, screenHeight,scale, renderer);
    }

    for (const auto &missile : SAMs) {
        missile.display(screenMinX, screenMinY, screenWidth, screenHeight,scale, renderer);
    }

    //Draw a selection box
    if (boxSelectionActive) {
        SDL_SetRenderDrawColor(renderer, 128, 128, 255, 64);

        int x0Screen = boxSelectionX0 * scale - screenMinX;
        int y0Screen = boxSelectionY0 * scale - screenMinY;

        int width;
        int height;
        if (x0Screen > userInputs.mouseXPx) {
            width = x0Screen - userInputs.mouseXPx;
            x0Screen = userInputs.mouseXPx;
        } else {
            width = userInputs.mouseXPx - x0Screen;
        }
        if (y0Screen > userInputs.mouseYPx) {
            height = y0Screen - userInputs.mouseYPx;
            y0Screen = userInputs.mouseYPx;
        } else {
            height = userInputs.mouseYPx - y0Screen;
        }

        SDL_Rect quad = {x0Screen, y0Screen, width, height};

        SDL_RenderDrawRect(renderer, &quad);
    }

    if (primarySelectedCity!=-1 && cities[primarySelectedCity].getOwner()==playerCountryId) {
        cities[primarySelectedCity].displayInfobox(cityColonTexture,
            provinceColonTexture,
            coreColonTexture,
            ownerColonTexture,
            developmentColonTexture,
            developTexture,
            changeSpecializationTexture,
            specializationColonTexture,
            airDefenceColonTexture,
            yesTexture,
            noTexture,
            incomeColonTexture,
            euroTexture,
            armyCapColonTexture,
            stockpileColonTexture,
            productionColonTexture,
            factoryTextTexture,
            missileSiteTextTexture,
            noneTextTexture,
            bulletsTexture,
            developMouseOverText,
            developMaxMouseOverText,
            addAirDefenceMouseOverText,
            countries,
            numbererSmall,
            userInputs.mouseXPx, userInputs.mouseYPx,
            screenWidth,screenHeight,backgroundScale,renderer);
    }

    //Draw missiles and aiming point, if we have any ready missiles
    if (missileMapMode) {


        double mouseXWorld = ((userInputs.mouseXPx + screenMinX) / scale);
        double mouseYWorld = ((userInputs.mouseYPx + screenMinY) / scale);

        bool hasMissile=false;

        for (int c : selectedCities) {
            if (cities[c].getOwner()==playerCountryId && cities[c].getSpecialization()==city::MISSILE_SITE && cities[c].hasMissileReady()) {


                double cx = cities[c].getX();
                double cy = cities[c].getY();

                int cxScreen = cx*scale-screenMinX;
                int cyScreen = cy*scale-screenMinY;

                double radiusScreen = missileRange*scale;
                double dtheta=1.0/std::max(1.0,std::floor((scale*64.0)))*M_PI;

                for (double theta = 0; theta<2*M_PI; theta+=dtheta) {
                    double posX = cxScreen + radiusScreen*cos(theta);
                    double posY = cyScreen + radiusScreen*sin(theta);

                    circleMarkerTexture.render(posX,posY-circleMarkerTexture.getHeight()*0.5,255,0,0,renderer,1.0,true,false,false,1,0,theta);
                }

                double dx = mouseXWorld-cx;
                double dy = mouseYWorld-cy;

                if (dx*dx+dy*dy<missileRange*missileRange) {
                    cities[c].highlightDirectPathScreen(arrowTexture,userInputs.mouseXPx, userInputs.mouseYPx, screenMinX, screenMinY, screenWidth, screenHeight,scale,renderer,millis,255,0,0);
                    hasMissile = true;
                }
            }
            if (cities[c].getOwner()==playerCountryId && cities[c].getHasAntiAirLauncher()) {

                double cx = cities[c].getX();
                double cy = cities[c].getY();

                int cxScreen = cx*scale-screenMinX;
                int cyScreen = cy*scale-screenMinY;

                double radiusScreen = SAMRange*scale;
                double dtheta=1.0/std::max(1.0,std::floor((scale*64.0)))*M_PI;

                for (double theta = 0; theta<2*M_PI; theta+=dtheta) {
                    double posX = cxScreen + radiusScreen*cos(theta);
                    double posY = cyScreen + radiusScreen*sin(theta);

                    circleMarkerTexture.render(posX,posY-circleMarkerTexture.getHeight()*0.5,0,0,255,renderer,1.0,true,false,false,1,0,theta);
                }

                double dx = mouseXWorld-cx;
                double dy = mouseYWorld-cy;

                if (dx*dx+dy*dy<missileRange*missileRange) {
                    cities[c].highlightDirectPathScreen(arrowTexture,userInputs.mouseXPx, userInputs.mouseYPx, screenMinX, screenMinY, screenWidth, screenHeight,scale,renderer,millis,255,0,0);
                    hasMissile = true;
                }
            }
        }
        if (hasMissile)
            missileAimMarker.render(userInputs.mouseXPx-missileAimMarker.getWidth()*0.5*scale,userInputs.mouseYPx-missileAimMarker.getHeight()*0.5*scale,renderer,scale,false,false,false,1,0,millis*0.001);
    }

    topBar.display(renderer, userInputs.mouseXPx, userInputs.mouseYPx, screenWidth, screenHeight, numbererMid,
                   numbererSmall);
    bottomBar.display(renderer, userInputs.mouseXPx, userInputs.mouseYPx, screenWidth, screenHeight, numbererMid,
                   numbererSmall);

    if (countries[playerCountryId].hasRegularQueuedEvents()) {
        countries[playerCountryId].displayRegularEvents(renderer,screenHeight-bottomBar.getHeight());
    }

    if (paused)
        pausedText.render(screenWidth * 0.5, screenHeight * 0.5, renderer, backgroundScale, true, true);

    if (musicManagerOpen)
        muse.displayManager(renderer, userInputs.mouseXPx, userInputs.mouseYPx,  screenWidth, screenHeight, backgroundScale);
    else if (diploMenuOpen) {
        if (diploNegotiatingWith==-1)
            diploManager->displayMenu(playerCountryId,renderer,countries,userInputs.mouseXPx, userInputs.mouseYPx,  screenWidth, screenHeight, backgroundScale);
        else
            diploManager->displayNegotiations(playerCountryId,diploNegotiatingWith,renderer,numbererSmall,countries,userInputs.mouseXPx, userInputs.mouseYPx,  screenWidth, screenHeight, backgroundScale);
    }
    else if (countries[playerCountryId].hasPriorityQueuedEvents()) {
        countries[playerCountryId].showFirstEvent(renderer, userInputs.mouseXPx, userInputs.mouseYPx, screenWidth, screenHeight, backgroundScale,receivedMessage,okText,yesText,noText);
    }

    if (missileMapMode)
        missileMapModeText.render(0,topBar.getHeight(),255,0,0,renderer,backgroundScale);
    else if (supplyMapMode)
        supplyMapModeText.render(0,topBar.getHeight(),128,200,255,renderer,backgroundScale);
    else if (neighbourMapMode)
        neighbourMapModeText.render(0,topBar.getHeight(),255,128,0,renderer,backgroundScale);
}

void game::update(SDL_Renderer *renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight,
                  const inputData &userInputs, unsigned int millis, unsigned int pmillis, TTF_Font *smallFont,
                  TTF_Font *midFont, TTF_Font *largeFont, std::default_random_engine& generator, musicManager& muse) {

    double backgroundScale = std::max(screenWidth/double(loadingBackground.getWidth()),screenHeight /double(loadingBackground.getHeight()));

    //Do this before we update time, so it effects time this frame
    if (userInputs.spacePressed && !userInputs.prevSpacePressed) {
        togglePause();
    }
    unsigned int dmillis = firstUpdate || (paused || musicManagerOpen || diploMenuOpen || countries[playerCountryId].hasPriorityQueuedEvents()) ? 0 : millis - pmillis;
    gameRealTime += dmillis;

    std::chrono::milliseconds realElapsed{gameRealTime};
    std::chrono::milliseconds gameElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        realElapsed * timewarpFactor);
    std::chrono::sys_time<std::chrono::milliseconds> currentGameTime = gameEpoch + gameElapsed;
    unsigned int dtGameTime = (currentGameTime - previousGameTime).count();


    double dt = firstUpdate ? 0 : (dmillis) * 0.001;

    //Useful for debugging
    //std::cout<<mouseXWorld<<" "<<mouseYWorld<<std::endl;

    if (userInputs.leftPressed) {
        //Use millis and pmillis explicitly, since we allow motion while paused
        screenMinX -= (millis - pmillis);
    }
    if (userInputs.rightPressed) {
        screenMinX += (millis - pmillis);
    }
    if (userInputs.upPressed) {
        screenMinY -= (millis - pmillis);
    }
    if (userInputs.downPressed) {
        screenMinY += (millis - pmillis);
    }
    if (userInputs.zoomInPressed) {
        scaleExponent += (millis - pmillis) * 0.001;
        scaleExponent = scaleExponent > maxScaleExponent ? maxScaleExponent : scaleExponent;

        //The following relation exists:
        //xScreen = xWorld*scale-screenMinX;
        //And we want the centre of the screen to stay fixed, i.e. xWorld should be unchanged for
        //windowWidthPx/2= xWorld*scale-screenMinX;
        //So
        double centreXWorld = ((screenWidth / 2.0 + screenMinX) / scale);
        //And
        double centreYWorld = ((screenHeight / 2.0 + screenMinY) / scale);
        scale = std::pow(2.0, scaleExponent);


        if (scale>=1) {
            shotSound.attenuate(1.0);
        }
        else if (scale<0.5) {
            shotSound.attenuate(0.0);
        }
        else {
            shotSound.attenuate(2*(scale-0.5));
        }


        //Now we still want
        //windowWidthPx/2= xWorld*scale-screenMinX;
        //So
        screenMinX = centreXWorld * scale - screenWidth / 2.0;
        screenMinY = centreYWorld * scale - screenHeight / 2.0;
    }
    if (userInputs.zoomOutPressed) {
        scaleExponent -= (millis - pmillis) * 0.001;
        scaleExponent = scaleExponent < minScaleExponent ? minScaleExponent : scaleExponent;
        //The following relation exists:
        //xScreen = xWorld*scale-screenMinX;
        //And we want the centre of the screen to stay fixed, i.e. xWorld should be unchanged for
        //windowWidthPx/2= xWorld*scale-screenMinX;
        //So
        double centreXWorld = ((screenWidth / 2.0 + screenMinX) / scale);
        //And
        double centreYWorld = ((screenHeight / 2.0 + screenMinY) / scale);
        scale = std::pow(2.0, scaleExponent);
        //Now we still want
        //windowWidthPx/2= xWorld*scale-screenMinX;
        //So
        screenMinX = centreXWorld * scale - screenWidth / 2.0;
        screenMinY = centreYWorld * scale - screenHeight / 2.0;


        if (scale>=1) {
            shotSound.attenuate(1.0);
        }
        else if (scale<0.5) {
            shotSound.attenuate(0.0);
        }
        else {
            shotSound.attenuate(2*(scale-0.5));
        }

    }
    if (userInputs.homePressed && !userInputs.prevHomePressed) {
        double cameraCentreX = 0;
        double cameraCentreY = 0;
        int playerCities = 0;

        for (auto &city: cities) {
            if (city.getOwner() == playerCountryId) {
                cameraCentreX += city.getX();
                cameraCentreY += city.getY();
                playerCities++;
            }
        }
        if (playerCities>0) {
            cameraCentreX /= playerCities;
            cameraCentreY /= playerCities;
        }

        //Centre the camera on the player country
        screenMinX = cameraCentreX * scale - screenWidth/ 2.0;
        screenMinY = cameraCentreY * scale - screenHeight/ 2.0;

    }

    double mouseXWorld = ((userInputs.mouseXPx + screenMinX) / scale);
    double mouseYWorld = ((userInputs.mouseYPx + screenMinY) / scale);

    bool missileMapMode=false;
    bool supplyMapMode=false;
    bool neighbourMapMode=false;

    if (userInputs.mPressed)
        missileMapMode = true;
    else if (userInputs.sPressed)
        supplyMapMode = true;
    else if (userInputs.nPressed)
        neighbourMapMode = true;


    //If we are in missile map mode, we will NOT be able to select anything
    if (missileMapMode) {
        //Manually launch missiles
        if (userInputs.leftMouseDown && !userInputs.prevLeftMouseDown) {

            //Find the nearest city, to see if we are allowed to launch
            int nearestCity=-1;
            double nearestCityDistance2=std::numeric_limits<double>::max();
            for (int i = 0; i < cities.size(); i++) {
                const auto &city = cities[i];
                double cx = city.getX();
                double cy = city.getY();
                double dx = cx-mouseXWorld;
                double dy = cy-mouseYWorld;
                double dist2 = (dx*dx + dy*dy);
                if (dist2 < nearestCityDistance2) {
                    nearestCityDistance2=dist2;
                    nearestCity=i;
                }
            }

            bool canLaunch=true;
            if (nearestCity!=-1) {
                if (cities[nearestCity].getOwner()==playerCountryId) {
                    //Sure, go for it, if you feel like it
                    //Maybe there are invading armies about
                    canLaunch=true;
                }
                else if (!countries[playerCountryId].atWarWith(cities[nearestCity].getOwner(),*diploManager)) {
                    canLaunch=false;
                    countries[playerCountryId].enqueueMessage(eventMessage("cantLaunchAtNeutral",cities[nearestCity].getOwner(),playerCountryId,true,false));
                }
            }

            if (canLaunch)
                for (int c : selectedCities) {
                    if (cities[c].getOwner()==playerCountryId && cities[c].getSpecialization()==city::MISSILE_SITE && cities[c].hasMissileReady()) {

                        double dx = mouseXWorld-cities[c].getX();
                        double dy = mouseYWorld-cities[c].getY();

                        if (dx*dx+dy*dy<missileRange*missileRange) {
                            cities[c].launchMissile();
                            missiles.emplace_back(missileInAir,cities[c].getX(),cities[c].getY(),mouseXWorld,mouseYWorld,countries[playerCountryId].getMissileSpeed(),playerCountryId);
                        }
                    }
                }
        }
    } else {
        //If we make a development click, we shouldn't deselect the city
        bool madeDevClick=false;
        if (primarySelectedCity!=-1 && cities[primarySelectedCity].getOwner()==playerCountryId) {
            auto hoveredInfo = cities[primarySelectedCity].updateInfobox(
                cityColonTexture,
                provinceColonTexture,
                coreColonTexture,
                ownerColonTexture,
                developmentColonTexture,
                developTexture,
                changeSpecializationTexture,
                specializationColonTexture,
                airDefenceColonTexture,
                yesTexture,
                noTexture,
                incomeColonTexture,
                euroTexture,
                armyCapColonTexture,
                stockpileColonTexture,
                productionColonTexture,
                factoryTextTexture,
                missileSiteTextTexture,
                noneTextTexture,
                bulletsTexture,
                countries,
                numbererSmall,
                userInputs.leftMouseDown && !userInputs.prevLeftMouseDown, userInputs.mouseXPx, userInputs.mouseYPx,
                screenWidth,screenHeight,backgroundScale);

            if (userInputs.leftMouseDown && !userInputs.prevLeftMouseDown) {
                if (hoveredInfo==city::DEVELOP_HOVER) {
                    if (countries[playerCountryId].getFunds()>=cities[primarySelectedCity].getDevCost()) {
                        countries[playerCountryId].spendFunds(cities[primarySelectedCity].getDevCost());
                        cities[primarySelectedCity].incrementDevelopment();
                    }
                    madeDevClick=true;
                }
                else if (hoveredInfo==city::FACTORY_HOVER) {
                    if (countries[playerCountryId].getFunds()>=cities[primarySelectedCity].getRespecCost()) {
                        countries[playerCountryId].spendFunds(cities[primarySelectedCity].getRespecCost());
                        cities[primarySelectedCity].setSpecialization(city::FACTORY);
                    }
                    madeDevClick=true;
                }
                else if (hoveredInfo==city::MISSILE_HOVER) {
                    if (countries[playerCountryId].getFunds()>=cities[primarySelectedCity].getRespecCost()) {
                        countries[playerCountryId].spendFunds(cities[primarySelectedCity].getRespecCost());
                        cities[primarySelectedCity].setSpecialization(city::MISSILE_SITE);
                    }
                    madeDevClick=true;
                }
                else if (hoveredInfo==city::AIRDEFENCE_HOVER) {
                    if (countries[playerCountryId].getFunds()>=cities[primarySelectedCity].getAirDefenceCost()) {
                        countries[playerCountryId].spendFunds(cities[primarySelectedCity].getAirDefenceCost());
                        cities[primarySelectedCity].buildAirDefence();
                    }
                    madeDevClick=true;
                }
            }
        }



        //Update which cities are selected by the player
        int prevPrimarySelectedCity = primarySelectedCity;
        //Left click to select cities
        if (userInputs.leftMouseDown && !userInputs.prevLeftMouseDown && !madeDevClick) {
            //You need shift click to keep the selection
            if (!userInputs.shiftPressed) {
                primarySelectedCity = -1;
                selectedCities.clear();
            }

            //Select any city which the mouse is over, and which is either ours or has soldiers loyal to us
            for (int i = 0; i < cities.size(); i++) {
                const auto &city = cities[i];
                if (city.isSelected(cityTexture, userInputs.mouseXPx, userInputs.mouseYPx, screenMinX, screenMinY, scale)
                    && (city.getOwner() == playerCountryId || city.hasSoldiersFrom(playerCountryId))
                ) {
                    primarySelectedCity = i;
                    selectedCities.insert(i);
                }
            }

            boxSelectionActive = true;
            boxSelectionX0 = mouseXWorld;
            boxSelectionY0 = mouseYWorld;
        }

        //Finish box selection
        if (userInputs.prevLeftMouseDown && !userInputs.leftMouseDown) {
            boxSelectionActive = false;
            //Check if there are any cities to select betwixt these coordinates
            double minX;
            double maxX;
            double minY;
            double maxY;
            if (mouseXWorld < boxSelectionX0) {
                minX = mouseXWorld;
                maxX = boxSelectionX0;
            } else {
                minX = boxSelectionX0;
                maxX = mouseXWorld;
            }
            if (mouseYWorld < boxSelectionY0) {
                minY = mouseYWorld;
                maxY = boxSelectionY0;
            } else {
                minY = boxSelectionY0;
                maxY = mouseYWorld;
            }
            //Select any city which the mouse is over, and which is either ours or has soldiers loyal to us
            for (int i = 0; i < cities.size(); i++) {
                const auto &city = cities[i];
                if ((city.getX() > minX && city.getX() < maxX && city.getY() > minY && city.getY() < maxY)
                    && (city.getOwner() == playerCountryId || city.hasSoldiersFrom(playerCountryId))
                ) {
                    primarySelectedCity = i;
                    selectedCities.insert(i);
                }
            }
        }

        //Right click to move
        if (userInputs.rightMouseDown && !userInputs.prevRightMouseDown) {
            //First try to move soldiers to direct neighbours
            for (int i: selectedCities) {
                auto &city = cities[i];
                city.moveSoldiersTo(playerCountryId, hoveredCity, userInputs.shiftPressed, cities, countries, tickets,*diploManager);
            }
        }

        //In any case, update which city we are hovering over
        int prevHoveredCity = hoveredCity;
        hoveredCity = -1;

        for (int i = 0; i < cities.size(); i++) {
            const auto &city = cities[i];
            if (city.isSelected(cityTexture, userInputs.mouseXPx, userInputs.mouseYPx, screenMinX, screenMinY, scale)) {
                hoveredCity = i;
            }
        }

        if (hoveredCity != -1 && primarySelectedCity != -1) {
            if (prevPrimarySelectedCity != primarySelectedCity || prevHoveredCity != hoveredCity) {
                selectedPath = cities[hoveredCity].findPathFrom(primarySelectedCity, cities, countries);
            }
        } else if (!selectedPath.empty())
            selectedPath.clear();
    }


    topBar.updateMouse(userInputs.mouseXPx, userInputs.mouseYPx, userInputs.leftMouseDown && !userInputs.prevLeftMouseDown, userInputs.rightMouseDown && !userInputs.prevRightMouseDown, screenWidth, screenHeight);
    bottomBar.updateMouse(userInputs.mouseXPx, userInputs.mouseYPx, userInputs.leftMouseDown && !userInputs.prevLeftMouseDown, userInputs.rightMouseDown && !userInputs.prevRightMouseDown, screenWidth, screenHeight);


    //Update projectiles and particle effects
    for (auto &shot: smallArmsShots)
        shot.update(dt);
    for (auto& explosion: explosions)
        explosion.update(dtGameTime);

    //Clear away dead particle effects
    //Clear lingering shots, since they are inserted from the back, and have the same lifetime, the expired shots are all up front
    while (!smallArmsShots.empty() && smallArmsShots.front().dead())
        smallArmsShots.pop_front();

    //Update which tiles are in view
    for (auto &tile: tiles) {
        tile->update(screenMinX, screenMinX + screenWidth, screenMinY, screenMinY + screenHeight, scale, renderer);
    }

    //Update logistics (trains with passengers or goods)
    for (auto &ticket: tickets) {
        ticket.update(cities, countries, dt,*diploManager);
    }
    for (auto &ticket: cargoTickets) {
        ticket->update(cities, countries, dt,*diploManager);
    }

    for (auto &sam : SAMs) {
        sam.update(dtGameTime);
        if (sam.hasHit()) {
            double tx = sam.getTargetX();
            double ty = sam.getTargetY();
            explosions.emplace_back(bigExplosion,tx,ty,missileExplosionRadius);

            explosionSound.play(tx,ty,screenMinX,screenMinY,screenWidth,screenHeight,scale,false);
            double missileExplosionRadius2= missileExplosionRadius*missileExplosionRadius;
            for (auto &missile : missiles) {
                double dx = missile.getX()-tx;
                double dy = missile.getY()-ty;
                if (dx*dx+dy*dy <missileExplosionRadius2) {
                    missile.shootDown();
                }
            }
        }
    }


    for (auto &missile : missiles) {
        missile.update(dtGameTime);
        if (missile.hasHit()) {
            double tx = missile.getTargetX();
            double ty = missile.getTargetY();
            explosions.emplace_back(bigExplosion,tx,ty,missileExplosionRadius);

            explosionSound.play(tx,ty,screenMinX,screenMinY,screenWidth,screenHeight,scale,false);
            //Check what sort of damage we should do (The explosion effect is purely graphical, THIS is where the damage is done)
            double missileExplosionRadius2= missileExplosionRadius*missileExplosionRadius;
            for (auto &s : soldiers) {
                double dx = s->getX()-tx;
                double dy = s->getY()-ty;

                if (dx*dx+dy*dy <missileExplosionRadius2) {
                    s->kill(countries);
                }
            }

            for (auto &city : cities) {
                double dx = city.getX()-tx;
                double dy = city.getY()-ty;

                if (dx*dx+dy*dy <missileExplosionRadius2) {
                    city.damage(countries);
                }
            }

            for (auto &ticket : tickets) {
                if (ticket.isInRangeOf(cities,tx,ty,missileExplosionRadius)) {
                    ticket.destroy(cities,countries,*diploManager);
                }
            }

            for (auto &ticket : cargoTickets) {
                if (ticket->isInRangeOf(cities,tx,ty,missileExplosionRadius)) {
                    ticket->destroy();
                }
            }

            for (auto &city : cities) {
                city.removeDeadSoldiers(cities, countries,*diploManager);
            }
        }
    }

    while (!explosions.empty() && explosions.front().isDead())
        explosions.pop_front();


    SAMs.remove_if([](const missile& missile){return missile.isDead();});
    missiles.remove_if([](const missile& missile){return missile.isDead();});
    tickets.remove_if([](const ticket &ticket) { return ticket.isDone(); });
    cargoTickets.remove_if([](const std::shared_ptr<cargoTicket> &ticket) { return ticket->isDone(); });

    std::vector<std::shared_ptr<countryball> > shotBalls;
    for (auto &ball: soldiers) {
        if (countries[ball->getAllegiance()].isDead()) {
            ball->kill(countries);
            int base = ball->getBase();
            if (base >= 0 && base < cities.size())
                cities[base].removeDeadSoldiers(cities, countries,*diploManager);
        }
        ball->move(dt, movementPenalties, watermap);
        ball->shoot(shotBalls, smallArmsShots, soldiers, cities, generator, dt,*diploManager,shotSound,screenMinX, screenMinY, screenWidth, screenHeight, scale);
        ball->reload(cities);

    }

    //Missiles currently at the ready, indexed by country/cities
    std::map<int,std::vector<int>> readyMissiles;

    std::map<int,std::vector<int>> readySAMS;

    for (int i = 0; i < cities.size(); ++i) {
        auto &city = cities[i];
        city.shoot(countries,shotBalls, smallArmsShots, soldiers, cities, generator, dt,*diploManager,shotSound,screenMinX, screenMinY, screenWidth, screenHeight, scale);
        city.updateOwnership(cities, countries,supplyHubs,*diploManager);
        if (city.updateRecruitment(dtGameTime)) {
            countries[city.getOwner()].decrementRecruitingSoldiers();
            soldiers.emplace_back(std::make_shared<countryball>(countries[city.getOwner()], city.getX(), city.getY(),0));
            city.addCountryball(soldiers.back(), cities, countries,*diploManager);
        }
        if (city.updateMissileBuilding(dtGameTime)) {
            //Currently nothing is done on our side
            //std::cout<<"Build missile in "<<city.getName()<<std::endl;
        }
        if (city.updateSAMBuilding(dtGameTime)) {
            //Currently nothing is done on our side
            //std::cout<<"Build missile in "<<city.getName()<<std::endl;
        }

        if (city.hasMissileReady()) {
            if (!readyMissiles.contains(city.getOwner())) {
                readyMissiles.emplace(city.getOwner(),std::vector<int>{i});
            }
            else {
                readyMissiles.at(city.getOwner()).emplace_back(i);
            }
        }

        if (city.hasSAMReady()) {
            if (!readySAMS.contains(city.getOwner())) {
                readySAMS.emplace(city.getOwner(),std::vector<int>{i});
            }
            else {
                readySAMS.at(city.getOwner()).emplace_back(i);
            }
        }
        if (city.canRepair()) {
            city.repair();
            //It is not possible to get negative funds, so the repair is essentially free if you are broke
            countries[city.getOwner()].spendFunds(city.getRepairCost());
        }
        city.updateRepair(dtGameTime);
    }

    for (auto &ball: shotBalls) {
        ball->kill(countries);
        int base = ball->getBase();
        if (base >= 0 && base < cities.size())
            cities[base].removeDeadSoldiers(cities, countries,*diploManager);
    }

    for (int i = soldiers.size() - 1; i >= 0; i--) {
        if (soldiers[i]->shouldDespawn())
            soldiers.erase(soldiers.begin() + i);
    }



    //Update income, if the month has changed
    //A month takes about a minute, so checking for multiple months is not necessary
    auto prevDays = floor<std::chrono::days>(previousGameTime);
    auto currDays = floor<std::chrono::days>(currentGameTime);

    std::chrono::year_month_day prevYMD{prevDays};
    std::chrono::year_month_day currYMD{currDays};

    bool monthChanged =
            (prevYMD.year() != currYMD.year()) ||
            (prevYMD.month() != currYMD.month());

    bool dayChanged = monthChanged || (prevYMD.day() != currYMD.day());



    if (monthChanged || firstUpdate) {
        //Update taxes of all countries
        //First reset the memory of last month taxes, this is the last month now
        for (auto &country: countries) {
            country.resetLastMonthFundSources();
        }

        for (const auto &city: cities) {
            auto &thisContry = countries[city.getOwner()];
            if (city.getOwner() == city.getCore()) {
                thisContry.addFunds(city.getIncome() * thisContry.getCoreIncomeMultiplier(), 0, 0);
            } else {
                thisContry.addFunds(0, city.getIncome() * thisContry.getOccupiedIncomeMultiplier(), 0);
            }
        }
        //Then subtract soldier upkeep cost, and limit to max funds
        for (int i = 0; i < countries.size(); ++i) {
            auto &country = countries[i];
            //TODO, replace with different types of soldiers ... maybe
            country.addFunds(0, 0, country.getArmySize() * country.getSoldierUpkeepCost());

            if (country.limitFunds()) {
                country.enqueueMessage(eventMessage("welfare",i,i,true,false));
            }
        }


    }


    //AI an automation related scripts below:

    //Update auto recruitment
    for (int i = 0; i < countries.size(); ++i) {

        //Only the player has the option of
        //TODO, consider changing this when you add the AI, so the AI can turn of recruitment when at peace
        if (i==playerCountryId && autoRecruitMenu->getSelectedMenu()==2)
            continue;

        auto &country = countries[i];
        int armyDeficit = country.getArmyCap() - country.getArmySize() - country.getRecruitingSoldiers();

        if (armyDeficit > 0) {
            //Traverse the cities we can recruit in, in random order
            auto cores =country.getCoreIds();
            std::shuffle(cores.begin(), cores.end(), generator);
            for (int coreId: cores) {
                if (country.getFunds() >= country.getInfantryRecruitmentCost()) {
                    //We can only recruit if we can afford it, the core is un-occupied, and not already recruiting
                    if (cities[coreId].getOwner() == country.getId())
                        if (cities[coreId].recruit(countries)) {
                            --armyDeficit;

                            country.spendFunds(country.getInfantryRecruitmentCost());
                            if (armyDeficit <= 0)
                                break;
                        }
                } else
                    break;;
            }
        }
    }

    //Update automatic building of missiles, don't bother shuffling, too much hassle
    for (auto& city : cities) {
        if (city.getSpecialization()==city::MISSILE_SITE) {
            auto& owner = countries[city.getOwner()];
            if (owner.getFunds()>owner.getMissileBuildingCost()) {
                if (city.buildMissile(countries)) {
                    owner.spendFunds(owner.getMissileBuildingCost());
                }
            }
        }
        if (city.getHasAntiAirLauncher()) {
            auto& owner = countries[city.getOwner()];
            if (owner.getFunds()>owner.getSAMBuildingCost()) {
                if (city.buildSAM(countries)) {
                    owner.spendFunds(owner.getSAMBuildingCost());
                }
            }
        }

    }

    for (auto& [countryId,SAMCities] : readySAMS) {

        if (!diploManager->isAtWar(countryId,countries))
            continue;

        double SAMRange2 = SAMRange*SAMRange;
        for (int i : SAMCities) {
            city& launchCity = cities[i];
            double x0 = launchCity.getX();
            double y0 = launchCity.getY();

            for (missile &m : missiles) {
                //This line prevents us from shooting down our own rockets
                if (!countries[m.getCountryId()].atWarWith(countryId,*diploManager))
                    continue;

                double mx = m.getX();
                double my = m.getY();

                //Vector from shooter to target
                double rx = mx-x0 ;
                double ry = my-y0;
                //This represents the missile being in-range of our radar, we will try to get an intercept solution
                if (rx*rx + ry*ry < SAMRange2) {
                    //Intercept calculation, pretty basic stuff just google it

                    double SAMSpeed = countries[countryId].getSAMSpeed();
                    double ssmVx = m.getVX();
                    double ssmVy = m.getVY();

                    //2nd degree equation to get the inverse of the time to target
                    //Has up to two solutions, but maybe 0
                    //First get the discriminant
                    double discriminant = 4*pow(rx*ssmVx+ry*ssmVy,2)-4*(rx*rx+ry*ry)*((ssmVx*ssmVx+ssmVy*ssmVy)-SAMSpeed*SAMSpeed);

                    //Negative time indicates no solution found
                    double interceptTime=-1;
                    double invInterceptTime=-1;
                    if (discriminant>=0) {
                        double invInterceptTime0 = (-2*(rx*ssmVx+ry*ssmVy)-sqrt(discriminant))/(2*(rx*rx+ry*ry));
                        double invInterceptTime1 = (-2*(rx*ssmVx+ry*ssmVy)+sqrt(discriminant))/(2*(rx*rx+ry*ry));

                        if (invInterceptTime0>0 && invInterceptTime1>0) {
                            if (invInterceptTime0>invInterceptTime1) {
                                invInterceptTime=invInterceptTime0;
                            }
                            else {
                                invInterceptTime=invInterceptTime1;
                            }
                        }
                        else if (invInterceptTime0>0) {
                            invInterceptTime=invInterceptTime0;
                        }
                        else if (invInterceptTime1>0) {

                            invInterceptTime=invInterceptTime1;
                        }
                        interceptTime = 1/invInterceptTime;
                    }

                    //Intercept is in the future, we can shoot at it
                    if (interceptTime>0 && interceptTime*SAMSpeed <= SAMRange2) {

                        double samVx = ssmVx+rx*invInterceptTime;
                        double samVy = ssmVy+ry*invInterceptTime;
                        double tx = x0+samVx*interceptTime;
                        double ty = y0+samVy*interceptTime;

                        SAMs.emplace_back(SAMInAir,x0,y0,tx,ty,SAMSpeed,countryId);
                        launchCity.launchSAM();
                    }
                    break;
                }
            }
        }

    }

    //Update missile auto-targeting
    for (auto& [countryId,missileCities] : readyMissiles) {

        if (countryId==playerCountryId && autoMissileMenu->getSelectedMenu()==0)
            continue;

        if (!diploManager->isAtWar(countryId,countries))
            continue;

        //So that we don't target the same place twice with multiple missiles, for the exceptionally unlikely chance that multiple missiles are ready at the same time (mostly relevant if the player suddenly turns on auto-targeting)
        std::set<int> alreadyTargeted;

        double missileRange2 = missileRange * missileRange;
        for (int i : missileCities) {
            city& launchCity = cities[i];
            double x0 = launchCity.getX();
            double y0 = launchCity.getY();

            //Loop through all enemy cities in range, and make a list of the ones with most soldier
            int maxSoldiers=0;
            std::vector<int> targets;


            for (int j = 0; j < cities.size(); ++j) {
                //Skip targets which have been assigned to other missiles
                if (alreadyTargeted.contains(j))
                    continue;
                //Add a target if we are at war with them, and they are in range
                city& potentialTarget = cities[j];
                if (countries[launchCity.getOwner()].atWarWith(potentialTarget.getOwner(),*diploManager)) {
                    double tx = potentialTarget.getX();
                    double ty = potentialTarget.getY();
                    double dx = tx - x0;
                    double dy = ty - y0;
                    if (dx*dx + dy*dy < missileRange2) {
                        int enemySoldiers = potentialTarget.getSoldiers(potentialTarget.getOwner());
                        //Add to target list if this has as many soldiers as the target, clear targets if this is a new standard for size
                        if (enemySoldiers==maxSoldiers) {
                            targets.push_back(j);
                        }
                        else if (enemySoldiers>maxSoldiers) {
                            targets.clear();
                            targets.push_back(j);
                            maxSoldiers=enemySoldiers;
                        }
                    }
                }
            }


            int targetIndex;

            if (targets.size()==1) {
                targetIndex=targets.front();
            }
            else if (targets.empty()) {
                continue;
            }
            else {

                std::uniform_int_distribution<int> targetDistribution(0,targets.size()-1);
                targetIndex = targets[targetDistribution(generator)];
            }

            alreadyTargeted.insert(targetIndex);

            launchCity.launchMissile();
            missiles.emplace_back(missileInAir,x0,y0,cities[targetIndex].getX(),cities[targetIndex].getY(),countries[playerCountryId].getMissileSpeed(),countryId);

        }
    }


    //Update auto-balance fronts options
    //The player must manually request an auto-balance
    if (autoBalanceButton->getIsClicked()) {
        if (diploManager->isAtWar(playerCountryId,countries)) {
            balanceFrontLinesWar(playerCountryId);
        }
        else {

            balanceFrontLinesPeace(playerCountryId);
        }
    }

    if (musicManagerButton->getIsClicked()) {
        musicManagerOpen = !musicManagerOpen;
        if (musicManagerOpen)
            diploMenuOpen = false;
        diploNegotiatingWith=-1;
    }

    if (diplomacyButton->getIsClicked()) {
        diploMenuOpen = !diploMenuOpen;
        if (diploMenuOpen)
            musicManagerOpen = false;
        diploNegotiatingWith=-1;
    }

    auto previousDayIndex = floor<std::chrono::days>((previousGameTime)-gameEpoch).count();
    auto currentDayIndex = floor<std::chrono::days>((currentGameTime)-gameEpoch).count();

    auto prev5Days = previousDayIndex/5;
    auto curr5Days = currentDayIndex/5;

    //AI does stuff, and random tension changes happen every 5 day
    if ((prev5Days!=curr5Days) || firstUpdate) {
        diploManager->resetCooldown();
        //For the diplomacy to work, recalculate all neighbours
        recalculateNeighbours();

        for (country &country : countries) {
            //Might as well dump events we don't need anymore
            country.dumpRegularEvents();
        }


        //Check for unification event
        for (const auto& unification : unificationEvents) {
            for (int i : unification.second) {
                //We can unify with someone if we exist, they don't, and we control 80% of their cores
                if (!countries[i].isDead())
                    for (int j : unification.second) {
                        if (i!=j) {
                            if (countries[j].isDead()) {
                                //Do the expensive count
                                int theirTotalCores=0;
                                int ourOwnedTheirCores=0;
                                for (const auto& city : cities) {
                                    if (city.getCore()==j) {
                                        ++theirTotalCores;
                                        if (city.getOwner()==i) {
                                            ++ourOwnedTheirCores;
                                        }
                                    }
                                }
                                if (ourOwnedTheirCores*10>theirTotalCores*8) {
                                    for (auto& city : cities) {
                                        if (city.getCore()==j) {
                                            city.setCore(i);

                                            if (city.getOwner()==i) {
                                                countries[i].decrementOccupiedCities();
                                                countries[i].addCoreId(city.getId());
                                                countries[i].incrementCoreCities();
                                            }
                                        }
                                    }
                                    //Tell literally everyone
                                    for (auto & country : countries) {
                                        country.enqueueMessage(eventMessage(unification.first,i,j,true,false));
                                    }
                                }
                            }
                        }
                    }
            }
        }

        //There is a 1/5 chance of random tension events with any neighbour, for any country, and 1/50 for non-neighbours
        std::uniform_int_distribution<int> tensionEventChance(0, 10);
        //If we get a tension down event, there is an additional chance we will flip it to a tension up event, this bodge has been added to make war more common
        std::uniform_int_distribution<int> flipToBadChance(0, 1);
        std::uniform_int_distribution<int> notNeighbourTensionEventChance(0, 100);
        std::uniform_int_distribution<int> tensionUpDist(0, tensionUpEvents.size()-1);
        std::uniform_int_distribution<int> tensionDownDist(0, tensionDownEvents.size()-1);
        std::uniform_int_distribution<int> accidentalWarDist(0, accidentalWarEvents.size()-1);

        //There is a 1/50 chance of random war declaration if tension is "bad"
        //2/50 if tension us "very_bad"
        //5/50 if tension is "terrible"
        std::uniform_int_distribution<int> randomWarChance(0, 49);

        for (int i = 0; i < countries.size(); ++i) {
            if (countries[i].isDead()) {
                if (countryExisted[i]) {
                    //Tell everyone about our tragic demise
                    for (auto & country : countries) {
                        country.enqueueMessage(eventMessage("elimination",i,i,true,false));
                    }
                }
                countryExisted[i]=false;

            }
            else
                for (int j = i+1; j < countries.size(); ++j) {
                    if (!countries[j].isDead())
                    {
                        if (!countries[i].atWarWith(j,*diploManager)) {
                            int randomWarRoll = randomWarChance(generator);
                            bool randomWar = false;
                            if (countries[i].isNeighbour(j))
                                switch (diploManager->getTension(i,j)) {
                                    default:
                                        break;
                                    case diplomacyManager::BAD:
                                        if (randomWarRoll==0)
                                            randomWar = true;
                                        break;
                                    case diplomacyManager::VERY_BAD:
                                        if (randomWarRoll<=1)
                                            randomWar = true;
                                        break;
                                    case diplomacyManager::TERRIBLE:
                                        if (randomWarRoll<=4)
                                            randomWar = true;
                                        break;
                                }
                            if (randomWar) {
                                int event = accidentalWarDist(generator);

                                countries[i].enqueueMessage(eventMessage(accidentalWarEvents[event],j,i,true,false));
                                countries[j].enqueueMessage(eventMessage(accidentalWarEvents[event],i,j,true,false));
                                for (int k = 0; k < countries.size(); ++k) {
                                    if (k!=i && k!=j) {
                                        countries[k].enqueueMessage(eventMessage("declareWarAllAccident",i,j,true,false));
                                    }
                                }

                                diploManager->setTension(i,j,diplomacyManager::WAR);
                            }
                            else {//Only random relation changes if we didn't just go to war
                                int TensionChange = countries[i].isNeighbour(j)? tensionEventChance(generator) : notNeighbourTensionEventChance(generator);
                                if (TensionChange ==0) {

                                    int makeItBad = flipToBadChance(generator);
                                    if (makeItBad==0) {
                                        //tension down
                                        diploManager->decreaseTensions(i,j);
                                        int event = tensionDownDist(generator);
                                        countries[i].enqueueMessage(eventMessage(tensionDownEvents[event],j,i,false,false));
                                        countries[j].enqueueMessage(eventMessage(tensionDownEvents[event],i,j,false,false));
                                    }
                                    else {
                                        //Tension up
                                        diploManager->increaseTensions(i,j,false);
                                        int event = tensionUpDist(generator);
                                        countries[i].enqueueMessage(eventMessage(tensionUpEvents[event],j,i,false,false));
                                        countries[j].enqueueMessage(eventMessage(tensionUpEvents[event],i,j,false,false));

                                    }
                                }
                                else if (TensionChange == 1) {
                                    //Tension up
                                    diploManager->increaseTensions(i,j,false);
                                    int event = tensionUpDist(generator);
                                    countries[i].enqueueMessage(eventMessage(tensionUpEvents[event],j,i,false,false));
                                    countries[j].enqueueMessage(eventMessage(tensionUpEvents[event],i,j,false,false));
                                }
                            }
                        }
                    }
                }
            }

        //AI diplomacy, and front-line rebalancing


        std::cout <<"Balancing front-lines "<< std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < countries.size(); ++i) {
            if (i != playerCountryId) {
                if (diploManager->isAtWar(i,countries))
                    balanceFrontLinesWar(i);
                else
                    balanceFrontLinesPeace(i);
                if (countries[i].aiDiplomacy(eventMessages,countries,*diploManager,generator)) {
                    for (auto &city: cities) {
                        city.updateSoldierLocations(cities,countries,*diploManager);
                    }
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout <<"Balance front line took "<< duration.count() << " ms\n"<<std::endl;



    }

    if (stanceMenu->getSelectedMenu()==0) {
        countries[playerCountryId].setOffensiveStance(country::DEFENSIVE);
    }
    else if (stanceMenu->getSelectedMenu()==1) {
        countries[playerCountryId].setOffensiveStance(country::CAUTIOUS);
    }
    else if (stanceMenu->getSelectedMenu()==2) {
        countries[playerCountryId].setOffensiveStance(country::AGGRESSIVE);
    }


    //AI countries check for events and respond every frame
    for (int i = 0; i < countries.size(); i++) {
        if (i != playerCountryId)
        {
            if (countries[i].handleEvents(countries,*diploManager))
                for (auto &city: cities) {
                    city.updateSoldierLocations(cities,countries,*diploManager);
                }
        }
    }
    if (countries[playerCountryId].hasPriorityQueuedEvents()) {
        countries[playerCountryId].finalizePriorityEvents(eventMessages,countries,renderer,smallFont,screenWidth,screenHeight);
    }
    if (countries[playerCountryId].hasRegularQueuedEvents()) {
        countries[playerCountryId].finalizeRegularEvents(eventMessages,countries,renderer,smallFont,screenWidth,screenHeight);
    }


    //Attack decisions are taken daily, leading to maximum chaos, we also update army capacity daily
    if (dayChanged || firstUpdate)
    {
        //Recalculate army capacity every day
        for (auto& country : countries) {
            country.resetArmyCap();
        }
        for (auto &city: cities) {
            int owner = city.getOwner();
            if (owner == city.getCore())
                countries[owner].addArmyCap(city.getArmyCapacity(),0);
            else
                countries[owner].addArmyCap(0,city.getArmyCapacity());
        }

        //Update production in factories
        for (auto &city : cities) {
            city.updateDailyProduction(countries);
        }

        for (auto &supplyHub: supplyHubs | std::views::values) {
            supplyHub.supplyTick(cities,countries,supplyHubs,cargoTickets);
           // std::cout<<"There are "<<cargoTickets.size()<<" cargo tickets active"<<std::endl;
        }


        //Update which cities should auto-attack which neighbours
        for (auto &city: cities) {
            int owner= city.getOwner();
            auto stance = countries[owner].getOffensiveStance();
            if (stance!=country::DEFENSIVE) {
                for (int n : city.getNeighbours()) {
                    int ourSoldiers = city.getSoldiers(owner);
                    int theirOwner = cities[n].getOwner();
                    if (countries[owner].atWarWith(theirOwner,*diploManager)) {
                        int theirSoldiers = cities[n].getSoldiers(theirOwner);
                        bool shouldAttack= (stance == country::AGGRESSIVE && theirSoldiers*2<ourSoldiers) || (stance == country::CAUTIOUS && theirSoldiers*4<ourSoldiers);
                        if (shouldAttack)
                            city.moveSoldiersTo(owner,n,false,cities,countries,tickets,*diploManager);
                    }
                }
            }
        }
    }



    //Update UI counters
    calendar->setTime(currentGameTime);
    cityCounter->setCount(countries[playerCountryId].getOccupiedCities(), countries[playerCountryId].getCoreCities());
    armyCapCounter->setCount(countries[playerCountryId].getArmySize(),
                             countries[playerCountryId].getRecruitingSoldiers(),
                             countries[playerCountryId].getArmyCap(), countries[playerCountryId].getArmyCapCores(),
                             countries[playerCountryId].getArmyCapOccupied());
    fundsTracker->setValues(countries[playerCountryId]);

    //Update music manager, if it is open
    if (musicManagerOpen) {
        muse.updateManager(userInputs.mouseXPx,userInputs.mouseYPx,userInputs.leftMouseDown,userInputs.leftMouseDown && !userInputs.prevLeftMouseDown,screenWidth,screenHeight,backgroundScale);
    }
    //Update diplomacy menu, if that thing is open
    else if (diploMenuOpen) {
        if (diploNegotiatingWith==-1)
            diploNegotiatingWith=diploManager->updateMenu(playerCountryId,countries,userInputs.leftMouseDown && !userInputs.prevLeftMouseDown, userInputs.mouseXPx, userInputs.mouseYPx,screenWidth,screenHeight,backgroundScale);
        else
            if (diploManager->updateNegotiations(playerCountryId,diploNegotiatingWith,countries,userInputs.leftMouseDown && !userInputs.prevLeftMouseDown,userInputs.mouseXPx, userInputs.mouseYPx,  screenWidth, screenHeight, backgroundScale)) {
                for (auto &city: cities) {
                    city.updateSoldierLocations(cities,countries,*diploManager);
                }
            }

    }
    else if (countries[playerCountryId].hasPriorityQueuedEvents()) {
        switch (countries[playerCountryId].updateFirstEvent(userInputs.leftMouseDown && !userInputs.prevLeftMouseDown, userInputs.mouseXPx, userInputs.mouseYPx, screenWidth, screenHeight, backgroundScale,okText,yesText,noText,countries,*diploManager)) {
            case eventMessage::NONE:
            default:
                //Nothing, the event is still present
            break;
                //TODO, these results are unused, maybe remove them
            case eventMessage::YES:
                //Just to be safe, update soldier locations
                for (auto &city: cities) {
                    city.updateSoldierLocations(cities,countries,*diploManager);
                }
            case eventMessage::OK:
            case eventMessage::NO:
                //Pop the event and move on
                countries[playerCountryId].handledFirstEvent();
            break;

        }
    }

    if (userInputs.escapePressed) {
        musicManagerOpen = false;
        diploMenuOpen = false;
        diploNegotiatingWith=-1;
    }


    if (framesSinceFPSprint >= 100) {
        unsigned deltaMillis = millis - previousFPSprintMillis;
        msPerFrame = deltaMillis / ((double) framesSinceFPSprint);
        std::cout << "ms per frame " << msPerFrame << std::endl;

        previousFPSprintMillis = millis;
        framesSinceFPSprint = 0;
    } else
        ++framesSinceFPSprint;

    previousGameTime = currentGameTime;
    firstUpdate = false;
}

bool game::shouldOpenNewScene(openSceneCommand &command, std::string &arguments) const {
    return false;
}

game::~game() {
    //The destructors of my stuff takes care of the cleanup
}

void game::balanceFrontLinesWar(int targetCountry) {
    //Loop over all cities belonging to this country
    //Count how many hostile neighbours they have
    //Then we can figure out how many soldiers to put down per front
    int totalHostileNeighbours=0;
    std::map<int,int> citiesWithHostileNeighbours;
    std::set<int> myCities;
    for (int i = 0; i < cities.size(); ++i) {
        const auto &city = cities[i];
        //TODO, we also need to consider soldiers in other cities than my own
        if (city.getOwner() == targetCountry) {
            int hostiles = city.getHostileNeighbours(cities,countries,*diploManager);
            totalHostileNeighbours+=hostiles;
            //We also insert cities with 0 hostile neighbours, because we will loop over them too
            citiesWithHostileNeighbours.emplace(i,hostiles);
        }
    }
    //No need to re-balance if we have no enemies
    if (totalHostileNeighbours==0)
        return;

    //Divide
    int armySize = countries[targetCountry].getArmySize();
    int averageSoldiersPerFront = armySize /totalHostileNeighbours;
    int remainder = armySize%totalHostileNeighbours;

    //This is the desired number of soldiers per each front
    std::map<int,int> citiesWithRequestedSoldiers;
    for (const auto &cityFronts : citiesWithHostileNeighbours) {
        int nSoldiers = averageSoldiersPerFront*cityFronts.second;
        //Get extra soldiers from the remainder
        if (remainder>0) {
            int extraSoldiers = std::min(cityFronts.second, remainder);
            remainder-=extraSoldiers;
            nSoldiers+=extraSoldiers;
        }
        //Excess need will be negative, if we have to many soldiers here
        int excessNeed = nSoldiers - cities[cityFronts.first].getSoldiers(targetCountry);
        citiesWithRequestedSoldiers.emplace(cityFronts.first, excessNeed);
    }
    //Run dijkstra's algorithm to find the paths
    frontlinePathByCountry[targetCountry] = getReinforcementPaths(citiesWithRequestedSoldiers, targetCountry);

    //Then loop through all cities and dispatch soldiers along the paths
    for (auto [city, requestedSoldiers]: citiesWithRequestedSoldiers) {
        //This city has soldiers to spa
        if (requestedSoldiers < 0) {
            if (frontlinePathByCountry[targetCountry].contains(city)) {
                if (frontlinePathByCountry[targetCountry][city]!=-1) {
                   std::vector<int> path;
                    for (int i =city; i!=-1; i=frontlinePathByCountry[targetCountry][i]) {
                        path.push_back(i);
                    }
                    cities[city].transferSoldiersTo(targetCountry,-requestedSoldiers ,path,cities,countries,tickets,*diploManager);
                }
            }
        }
    }
}

std::map<int, int> game::getReinforcementPaths(const std::map<int, int>& citiesWithRequestedSoldiers, int targetCountry) const {

    //I use a std::map to store the distances and prev we have access to,
    //This is because we only has access to a (hopefully) small subset of all the cities
    //For example, Denmark by default has access to cities with ID 0,1,2,3,4,42, and 43,
    //So there is no point in using a vector with 670 cities when we are looking at Denmark
    std::map<int,double> distances;
    std::map<int,int> prev;
    std::vector<int> Q;

    //Create distances, prev, and Q ONLY for the cities we can access, this will likely be much smaller than the entire list of cities
    for (int i = 0; i< cities.size(); ++i) {
        //Only if the country owning the city gives access to someone from source country is the city added
        if (countries[cities[i].getOwner()].hasAccess(targetCountry)) {
            Q.emplace_back(i);
            //-1 is a shorthand for undefined/no previous
            prev[i]=-1;
            //This number is larger than all real distances we can compare it to
            distances[i]=std::numeric_limits<double>::max();
        }
    }

    //Set everywhere which requests additional soldiers as the source
    for (auto [city,requestedSoldiers] : citiesWithRequestedSoldiers) {
        if (requestedSoldiers>0)
            distances[city]=0;
    }

    while (!Q.empty()) {
        //Find the element in Q with the smallest distance
        int smallestId = 0;
        for (int i = 0; i < Q.size(); ++i) {
            if (distances[Q[i]]<distances[Q[smallestId]])
                smallestId = i;
        }
        int u = Q[smallestId];

        Q.erase(Q.begin()+smallestId);

        //Loop through all neighbours of u, and their distances
        //(getNeighbourDistances gives a reference to a map<int,double> with neighbour ids and pre-computed distances)
        for (auto [v, dist] : cities[u].getNeighbourDistances()) {
            //Update their distances
            double alt = dist+distances[u];
            if (alt < distances[v]) {
                distances[v] = alt;
                prev[v] = u;
            }
        }
    }


    //Prev will function as the path from every location, to the source (the front-line)
    return prev;
}

void game::recalculateNeighbours() {
    for (auto& country : countries) {
        country.resetNeighbourIds();
    }

    for (const auto& city : cities) {
        for (int n : city.getNeighbours()) {
            int neighbourOwner = cities[n].getOwner();
            int ownOwner = city.getOwner();
            if (ownOwner!=neighbourOwner) {
                countries[ownOwner].addNeighbourId(neighbourOwner);
                countries[neighbourOwner].addNeighbourId(ownOwner);
            }
        }
    }
}

void game::balanceFrontLinesPeace(int targetCountry) {
    //Loop over all cities belonging to this country
    //Count how many potentially hostile neighbours they have
    //Then we can figure out how many soldiers to put down per front
    int totalHostileNeighbours=0;
    std::map<int,int> citiesWithHostileNeighbours;
    std::set<int> myCities;
    for (int i = 0; i < cities.size(); ++i) {
        const auto &city = cities[i];
        if (city.getOwner() == targetCountry) {
            int borders=0;
            for (int n : city.getNeighbours()) {
                if (cities[n].getOwner() != targetCountry) {
                    ++borders;
                }
            }
            totalHostileNeighbours+=borders;
            //We also insert cities with 0 hostile neighbours, because we will loop over them too
            citiesWithHostileNeighbours.emplace(i,borders);
        }
        else
            citiesWithHostileNeighbours.emplace(i,0);
    }
    //No need to re-balance if we have no borders
    if (totalHostileNeighbours==0)
        return;

    //Divide
    int armySize = countries[targetCountry].getArmySize();
    int averageSoldiersPerFront = armySize /totalHostileNeighbours;
    int remainder = armySize%totalHostileNeighbours;

    int nToMove;
    //This is the desired number of soldiers per each front
    std::map<int,int> citiesWithRequestedSoldiers;
    for (const auto &cityFronts : citiesWithHostileNeighbours) {
        int nSoldiers = averageSoldiersPerFront*cityFronts.second;
        //Get extra soldiers from the remainder
        if (remainder>0) {
            int extraSoldiers = std::min(cityFronts.second, remainder);
            remainder-=extraSoldiers;
            nSoldiers+=extraSoldiers;
        }
        //Excess need will be negative, if we have to many soldiers here
        int excessNeed = nSoldiers - cities[cityFronts.first].getSoldiers(targetCountry);

        nToMove+=std::abs(excessNeed);
        citiesWithRequestedSoldiers.emplace(cityFronts.first, excessNeed);
    }

    if (nToMove==0)
        return;

    for (auto &cityFront0 : citiesWithRequestedSoldiers) {
        if (cityFront0.second<0)
            for (auto &cityFront1 : citiesWithRequestedSoldiers) {
                if (cityFront0.second<0 && cityFront1.second>0) {
                    int toTransfer = std::min(-cityFront0.second,cityFront1.second);

                    std::vector<int> path {cityFront0.first,cityFront1.first};

                    cities[cityFront0.first].transferSoldiersTo(targetCountry,toTransfer,path,cities,countries,tickets,*diploManager,true);

                    cityFront0.second+=toTransfer;
                    cityFront1.second-=toTransfer;
                }
            }
    }
}
